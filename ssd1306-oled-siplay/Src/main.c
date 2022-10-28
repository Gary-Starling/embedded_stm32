  
  
  
/* Includes */
#include "main.h"
#include <stdio.h>
#include <string.h>
/* Includes */

I2C_HandleTypeDef hi2c1;



/* function's */
void SystemClock_Config(void);
static void MX_I2C1_Init(void);


/* function's */

/*externs*/
//Изображения
extern const uint8_t image_data_arrow_up[28];
extern const uint8_t image_data_arrow_down[28];
extern const uint8_t image_data_X[576];
extern uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];
/*externs*/


/*variables*/
#ifdef DEBUG
#endif
/*variables*/

int main(void) //Главный цикл

{
  
  char strBig[5];         //Массив под вывод Больших букв
  int test=0;
  
  HAL_Init();
  SystemClock_Config();
  MX_I2C1_Init();
  
  ssd1306_Init(1);                                     //Инит дисплея
  ssd1306_Fill(Black);                                 //Заполним эран белым
  
  /* Демонстрация */
  
  /* обрати внимание на правильную передачу высоты
  и ширины картинки, иначе на экране будет каша,
  а так же на позиционирование, при неправильно
  выставленном курсоре картинка может не вместиться*/
  drawBitmap(32, 0, image_data_X,  65, 64, White);     //Картинку загрузим в буфер
  ssd1306_UpdateScreen();                              //Обновить экран
  HAL_Delay(1000);
  ssd1306_Fill(Black);                                 //Затрём экран
  HAL_Delay(1000);
 
  
  drawBitmap(0, 0, image_data_arrow_up,  9, 14, White);  
  ssd1306_UpdateScreen();
  HAL_Delay(1000);
  ssd1306_Fill(Black);                                 
  HAL_Delay(1000);
  
  
  
  
  while (1)
  {
    
    Out_number_center(test, strBig, 0);
    
    test++;
    
    HAL_Delay(500);
    
    if(test>10)
      test=0;
    
    ssd1306_UpdateScreen();
    
  }
  /* USER CODE END 3 */
}




/**
* @brief System Clock Configuration
* @retval None
*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}



static void MX_I2C1_Init(void)
{
  
  
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0000020B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
  
  /* USER CODE END I2C1_Init 2 */
  
}


void Error_Handler(void)
{
  
}


/* Форматированный вывод, по центру
Внимание! Только для шрифта 30x51
num - число для вывода
str - указатель на строу
Для шрифта Font_30x45*/

#pragma optimize=none
uint8_t Out_number_center(int num, char* str, uint8_t y)
{
  
  uint8_t x;            //Позиция по x
  uint8_t str_cnt = 0;  //Количество символов
  
  sprintf(str,"%d",num);       //Число без знак
  
  str_cnt = strlen(str);      //Узнаем количество символов
  
  if(str_cnt>4)
    return 0;
  
  x = (128-(str_cnt*30))/2;      //Центруем строку по оси X
  
  fill_region(0, x, 0, 64,Black);//
  
  ssd1306_SetCursor(x,y);
  
  ssd1306_WriteStringHight(str,Font_30x45,White); 
  
  fill_region(x + (str_cnt*30), SSD1306_WIDTH, 0, 64,Black);
  
  return 1;
}

















