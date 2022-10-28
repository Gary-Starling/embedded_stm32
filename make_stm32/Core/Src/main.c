/*
  Проект МПРТ22-KTY-81-110
  31.05.22

  Вся работа с opensource toolchain:
  arm-none-gcc -
  make
  arm-gdb - Важно, под виндой нужно добавить в исключения firewall!!!
  openocd - налогично
  ide - vscode (настройки смотри .vscode/tasks -/settings -/c_cpp_por..)

  drivers:
  HAL_Driver
  CMSIS

  Можно было вырезать весь hal, он тут не нужен, но для быстроты использовали его.
  Проект старается быть написан в стиле интерфейсоф периферийных модулей, избегая лишних
  глобальных.
*/

#include <stdint.h>
#include <stm32f030x6.h>
#include <main.h>
#include <led.h>
#include <kty_81_110.h>
#include <relay.h>
#include <stdbool.h>
#include <Buttons_menu.h>
#include <Flash_EEPROM.h>

/* Global */
DMA_HandleTypeDef hdma_adc;
TIM_HandleTypeDef htim3; //, htim14;
ADC_HandleTypeDef hadc;
IWDG_HandleTypeDef hiwdg;

// termoregulator
volatile uint16_t adc_value[2] = {0}; //Сюда складываем значения из рег.ацп
Dt_states dt_state[2] = {OK_T, OK_T}; //Состояние датчиков
Dt_cnts cnts_dt[2] = {0};             //Счётчики аварий
int real_temp1 = 0, real_temp2 = 0;   //Температуры первого и второго каналов
uint8_t rel_state = 0x00;             //Состояние реле 0x00 - off1,1; 0x01 1on 2of; 0x03 1on 2on; 0x02 1of 2on

// menu
Menu_structure Menu = {
    .exit_cnt = 0,
    .mig = 0,
    .restart_cnt = 0,
    .t_disp_cnt = 150,
    .mode = 0,
    .state = 0,
    .ind = 0};

extern int Ustanovki[N_UST];
extern volatile uint16_t zum_del;
/* Global */

int main(void); //Вход в основную программу

/* peripherial functions */
static void System_Init(void);
static void TIM3_Init(void);
// static void TIM14_Init(void);
static void ADC_Init(void);
static void GPIO_Init(void);
void Watchdog_Init(void);
// static void MX_USART1_UART_Init();

/* peripherial functions */

void Error_Handler(void);

int main(void)
{

  int result = OK_T;

  HAL_Init();
  System_Init();

  GPIO_Init();
  Relay_init();
  // TIM14_Init();
  TIM3_Init();       //Таймер для индикации
  ADC_Init();        //Запуск АЦП, circular mode
  sevenled_init();   //Инит семисегментного индикатора
  Buttons_Init();    //Инит кнопок
  EEPROM_checking(); //Прочитать установки
  Watchdog_Init();

  /* Стартовые замеры.
     Сделано для того чтобы не было ложных срабатываний реле
     Не было отображения низкой температуры при набивке буфера */
  ledprint(" U1_1 ", Menu); //Вывод   
  for (uint32_t i = 0; i < 100; i++)
  {
    HAL_IWDG_Refresh(&hiwdg);
    result = get_temp(&real_temp1, DT_1, CORRECT1, &adc_value[DT_1]);
    scan_error(&dt_state[DT_1], &cnts_dt[DT_1], result);
    result = get_temp(&real_temp2, DT_2, CORRECT2, &adc_value[DT_2]);
    scan_error(&dt_state[DT_2], &cnts_dt[DT_2], result);
    my_delay(20);
  }


  while (1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    /* Замер канала 1, работа с реле 1 по температуре, проверка на ошибки */
    result = get_temp(&real_temp1, DT_1, CORRECT1, &adc_value[DT_1]);
    scan_error(&dt_state[DT_1], &cnts_dt[DT_1], result);

    /* Замер канала 2, работа с реле 2 по температуре, проверка на ошибки */
    result = get_temp(&real_temp2, DT_2, CORRECT2, &adc_value[DT_2]);
    scan_error(&dt_state[DT_2], &cnts_dt[DT_2], result);

    if (Menu.mode)
    {
      RELE1_RESET;
      RELE2_RESET;
    }
    else
    {
      if (dt_state[DT_1] == OK_T)
        Relay_work(real_temp1, TEMP1, HYST1, INV1, RELE1_Pin);
      else
        RELE1_RESET;

      if (dt_state[DT_2] == OK_T)
        Relay_work(real_temp2, TEMP2, HYST2, INV2, RELE2_Pin);
      else
        RELE2_RESET;

      rel_state = (RELE1_READ << 0 | RELE2_READ << 1); //Поле для вывода состояния реле

      if (dt_state[DT_1] != OK_T && dt_state[DT_2] != OK_T)
      {
        if (Menu.mig < 64)
        {
          ZUMMER_ON;
          zum_del = 15;
        }
      }
    }

    /* Сначала обработаем кнопки, в EXTI4_15_IRQHandler флажки */
    button_handler(&Menu);
    /* Вывод меню */
    Menu_work_ind(&Menu, real_temp1, real_temp2, dt_state, rel_state); //
  }
}

static void System_Init(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

static void TIM3_Init(void)
{

  __HAL_RCC_TIM3_CLK_ENABLE();

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 79;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 333;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  HAL_TIM_Base_Start_IT(&htim3); //Всё прошло успешно,запустим таймер
}

/*
static void TIM14_Init(void)
{
  __HAL_RCC_TIM14_CLK_ENABLE();

  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 7999;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 2;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim14);

  HAL_TIM_Base_Start_IT(&htim14);
}
*/

static void ADC_Init(void)
{

  ADC_ChannelConfTypeDef sConfig = {0};

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;

  HAL_ADC_Init(&hadc);

  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_6;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  HAL_ADCEx_Calibration_Start(&hadc);
  HAL_ADC_Start_DMA(&hadc, (uint32_t *)&adc_value, 2);
}

static void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = ZUMMER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ZUMMER_GPIO_Port, &GPIO_InitStruct);
  ZUMMER_OFF;
}

void Watchdog_Init(void)
{
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 40000 / 128;
  hiwdg.Init.Window = IWDG_WINDOW_DISABLE;

  HAL_IWDG_Init(&hiwdg);
}