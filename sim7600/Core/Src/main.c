#include "main.h"
#include <string.h>
#include <ctype.h>

/* headers */
#include "cmsis_os2.h"
#include "FreeRTOS.h"
/* my */
#include "RingBuff.h"
#include "sim76xx.h"
#include "internet.h"
#include "sms.h"
#include "dtmf.h"

typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;


/*======================  Sim7600 parser task   ======================*/
osThreadId_t sim76xxParserHandle;
uint32_t sim76xxParserBuffer[ 2048 ];          // 8Kbyte
osStaticThreadDef_t sim76xxParsCb;
const osThreadAttr_t Sim76xxParser_attributes =
{
  .name = "Sim76xxParser",
  .cb_mem = &sim76xxParsCb,
  .cb_size = sizeof(sim76xxParsCb),
  .stack_mem = &sim76xxParserBuffer[0],
  .stack_size = sizeof(sim76xxParserBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};

/*======================  Sim7600 extra msg task   ======================*/
osThreadId_t sim76xxExtraHandle;
uint32_t sim76xxExtraBuffer[ 2048 ];          // 8Kbyte
osStaticThreadDef_t sim76xxExtraCb;
const osThreadAttr_t Sim76xxExtra_attributes =
{
  .name = "Sim76xxExtra",
  .cb_mem = &sim76xxExtraCb,
  .cb_size = sizeof(sim76xxExtraCb),
  .stack_mem = &sim76xxExtraBuffer[0],
  .stack_size = sizeof(sim76xxExtraBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};


/*  ====================== System check task  ======================*/
osThreadId_t SystemCheckTaskHandle;
uint32_t SystemCheckTasTaskBuffer[ 2048 ];                 // 8 Kbyte
osStaticThreadDef_t SystemCheckTasTaskCb;

/*  Заполнение структуры для SystemCheck thread */
const osThreadAttr_t SystemCheck_attributes = {
  .name = "System_check",
  .cb_mem = &SystemCheckTasTaskCb,
  .cb_size = sizeof(SystemCheckTasTaskCb),
  .stack_mem = &SystemCheckTasTaskBuffer[0],
  .stack_size = sizeof(SystemCheckTasTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};


/* ====================== Worktask  ======================*/
osThreadId_t WorktaskHandle;
uint32_t WorktaskBuffer[ 2048 ];                //4 Kbyte
osStaticThreadDef_t WorktaskCb;
const osThreadAttr_t Worktask_attributes =
{
  .name = "Worktask",
  .cb_mem = &WorktaskCb,
  .cb_size = sizeof(WorktaskCb),
  .stack_mem = &WorktaskBuffer[0],
  .stack_size = sizeof(WorktaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};

/* ====================== Internet task  ======================*/
osThreadId_t NettaskHandle;
uint32_t NettaskBuffer[ 2048 ];                //4 Kbyte
osStaticThreadDef_t NettaskCb;
const osThreadAttr_t Nettask_attributes =
{
  .name = "Nettask",
  .cb_mem = &NettaskCb,
  .cb_size = sizeof(NettaskCb),
  .stack_mem = &NettaskBuffer[0],
  .stack_size = sizeof(NettaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};


/* ====================== Sim76xx uart mutex  ======================*/
osMutexId_t MutexSim76xxHandle;
osStaticMutexDef_t MutexSim76xxCb;
const osMutexAttr_t MutexSim76xx_attr =
{
  .name = "MutexSim76xx",
  .cb_mem = &MutexSim76xxCb,
  .cb_size = sizeof(MutexSim76xxCb)
};

/* ====================== Definitions for sim76xxSem  ======================*/
//Данный семафор нужен для того чтобы оповещать парсер sim76xx о наличии
//сообщений, когда сообщений нет, задача парсер Sim76xxParser, будет просто
//в спящем состоянии
osSemaphoreId_t sim76xxSemHandle;
osStaticSemaphoreDef_t SimSemCb;
const osSemaphoreAttr_t sim76xxSem_attributes = {
  .name = "sim76xxSem",
  .cb_mem = &SimSemCb,
  .cb_size = sizeof(SimSemCb),
};


/* Definitions for SimAnsSendQueue */
osMessageQueueId_t SimAnsSendQueueHandle;
uint8_t SimAnsSendQueueBuff[ 10 * sizeof( sAnswrObject ) ];
osStaticMessageQDef_t SimAnsSendQueueCb;
const osMessageQueueAttr_t SimAnsSendQueue_attributes = {
  .name = "SimAnsSendQueueQueue",
  .cb_mem = &SimAnsSendQueueCb,
  .cb_size = sizeof(SimAnsSendQueueCb),
  .mq_mem = &SimAnsSendQueueBuff,
  .mq_size = sizeof(SimAnsSendQueueBuff)
};

/* Definitions for SimExtraMsgQueue */
osMessageQueueId_t SimExtraMsgQueueHandle;
uint8_t SimExtraMsgQueueBuff[ 100 * sizeof( sExtraMsg ) ];
osStaticMessageQDef_t SimExtraMsgQueueCb;
const osMessageQueueAttr_t SimExtraAnsQueue_attributes = {
  .name = "SimExtraMsgQueue",
  .cb_mem = &SimExtraMsgQueueCb,
  .cb_size = sizeof(SimExtraMsgQueueCb),
  .mq_mem = &SimExtraMsgQueueBuff,
  .mq_size = sizeof(SimExtraMsgQueueBuff)
};



/* tasks func */
void SystemCheck(void *argument);          //Работа с памятью USB/at45/littlef(rw)
void WorkTask(void *argument);             //Логика(реле,терморегуляторы и каналы)
void Sim76xxParser(void *argument);        //Парсер uart sim76xx
void Sim76xxExtraMsg(void *argument);      //То что нужно отработать сразу
void InternetTask(void *argument);         //Для работы с тг


static void SystemClock_Config(void);
static void periph_clock_init(void);
static void prio_init(void);




/*
Hard cconfig
HSE - 8MHz
SYSCLK -168 MHz
APB1 - 42 MHz
APB1 timer - 84 MHz
APB2 - 84 MHz
APB2 - 168 MHz
USB - 48 MHz
SPI - 42 MHz APB2/2
xPortSysTickHandler 1kHz - 1msec
*/



int main(void)
{
  /* MCU Config */
  HAL_Init();
  SystemClock_Config();
  periph_clock_init();
  prio_init();

  osKernelInitialize();

  /* add mutexes */
  MutexSim76xxHandle = osMutexNew(&MutexSim76xx_attr);    //мьютекс uart запросов sim76x

  /* semaphores */
  sim76xxSemHandle = osSemaphoreNew(MAX_SEMAPHORE, 0, &sim76xxSem_attributes);

  /* Очередь для приёма от отправителя */
  SimAnsSendQueueHandle = osMessageQueueNew (10, sizeof(sAnswrObject), &SimAnsSendQueue_attributes);
  /* Очередеь экстрасообщений */
  SimExtraMsgQueueHandle = osMessageQueueNew (100, sizeof(sExtraMsg), &SimExtraAnsQueue_attributes);


  SystemCheckTaskHandle = osThreadNew(SystemCheck, NULL, &SystemCheck_attributes);
  WorktaskHandle = osThreadNew(WorkTask, NULL, &Worktask_attributes);
  sim76xxParserHandle = osThreadNew(Sim76xxParser, NULL, &Sim76xxParser_attributes);
  sim76xxExtraHandle = osThreadNew(Sim76xxExtraMsg, NULL, &Sim76xxExtra_attributes);
  NettaskHandle = osThreadNew(InternetTask, NULL, &Nettask_attributes);
  osKernelStart();
  while (1);
}




void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;

  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}



/* Тактовая для используемых модулей */
static void periph_clock_init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

/* Расстановка приоритетов используемых прерываний */
static void prio_init(void)
{
  HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);       //uart3
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 6, 1); //rx
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 6, 0); //tx
}





void SystemCheck(void *argument)
{
  eComRes eInit_res;    //результат инита sim76xx
  sExtraMsg init;


  osThreadSuspend(WorktaskHandle);
  osThreadSuspend(NettaskHandle);

  /* Инициализируем модуль sim76xx */
  eInit_res = sim76xx_init();

  if(eInit_res != AT_SEND_OK)
  {
    init.msg = ERR_INIT;
    osMessageQueuePut (SimExtraMsgQueueHandle, &init, 0, 0); //
    while(1);
  }

  /* Моргнём, сказав что инит модема прошёл успешно */
  sim76xx_led_blink(100, 0);

  osThreadResume(WorktaskHandle);
  osThreadResume(NettaskHandle);
  osThreadSuspend(SystemCheckTaskHandle);
  for(;;)
  {}
}


void WorkTask(void *argument)
{

  for(;;)
  {
    osDelay(10);
  }

}


/***************************GSM MODEM TASK***********************/
#define PASSWORD    (const char *)("4321")
void Sim76xxExtraMsg(void *argument)
{
  char rSms[SMS_BUF_SIZE] = {0};
  osStatus_t  osMesSt;                   //Статус для очереди
  sExtraMsg  Extra;                      //Принятый ответ
  eComRes eSimRes;                       //Результат работы в sim76xx
  static bool pass_enter = false;        //

  for(;;)
  {
    osMesSt = osMessageQueueGet(SimExtraMsgQueueHandle, &Extra, NULL, osWaitForever);

    if(osOK == osMesSt)
    {
      switch(Extra.msg)
      {
        /* Звонок, нужно снять трубку */
        case RING:
        eSimRes = sim76xx_answ_call(); //Сняли трубку
        //say.answer();
        break;

        /* Положили трубку */
        case END_CALL:
        //stop_play();
        dtmfPass(&Extra, PASSWORD, &pass_enter, true);     //очистить счётчики dtmf
        dtmfLed(&Extra, true);
        pass_enter = false;
        break;

        case MISSED_CALL:
        break;

        /* Обработаем DTMF */
        case RXDTMF:
        if(!pass_enter)
          dtmfPass(&Extra, PASSWORD, &pass_enter, false);
        else
          dtmfLed(&Extra, false);
        break;

        /* Пришло SMS (чтение-парсинг-действие)*/
        case SMS:
        if(AT_SEND_OK == readSms(Extra.val, rSms, SMS_BUF_SIZE))
        {
          parsingSms(rSms, SMS_BUF_SIZE);
          memset(rSms, 0x00, SMS_BUF_SIZE);
        }
        break;

        case NO_CARRIER:
        //stop_play();
        dtmfPass(&Extra, PASSWORD, &pass_enter, true);      //очистить счётчики dtmf
        dtmfLed(&Extra, true);
        pass_enter = false;
        break;

        case SIM_NOT_AVL:
        break;

        /* Сим карта не вставлена */
        case SIM_NOT_INS:
        while(1)
          sim76xx_led_blink(300, 1);
        break;

        case SMS_FULL:
        break;

        case ERR_INIT:
        while(1)
          sim76xx_led_blink(300, 0);
        break;

        default:
        //stop_play();
        dtmfPass(&Extra, PASSWORD, &pass_enter, true);      //очистить счётчики dtmf
        dtmfLed(&Extra, true);
        pass_enter = false;
        break;
      }
    }

    osThreadYield();
  }
}







/*
Задача парсер, отправляет ответы функции sim76xx_send_at_com и
задаче Sim76xxExtraMsg, в случае возникновения внеочередных сообщений.
Т.е. тех, которых мы не ждали в ответ.

Sim76xxParser получает семафор из USART3_IRQHandler, прерывание сигнализирует
о наличии данных, которые нужно обработать.
*/
void Sim76xxParser(void *argument)
{
  extern sRingBuff sim_buff;     //кольцевой буфер для uart sim76xx( idle irq )
  osStatus_t osSem_stat = osOK;
  for(;;)
  {
    osSem_stat = osSemaphoreAcquire(sim76xxSemHandle, osWaitForever);

    /* семафор из irq, модуль что-то прислал, есть что парсить */
    if(osOK == osSem_stat)
      sim76xx_parsing(&sim_buff);

    osThreadYield();
  }

}
/***************************GSM MODEM TASK***********************/


void InternetTask(void *argument)
{
  char * pfind; //для парсинга и поиска
  size_t https_cnt = 0;
  size_t st_id = 0, next_id = 0;
  char rec_buff[512];
  eNetAnsw net_res;

  osDelay(10000);

  net_res = tg_send_msg("Terminal online", strlen("Terminal online") + 1,\
    TOKEN, CHAT_ID);

  //TODO suspended
  //Stop this task when calling!!!
  if(net_res != NET_OK)
    while(1);

  /* Нужно найти id сессии  при включении */
  tg_update_id(&st_id, TOKEN, rec_buff, sizeof(rec_buff));

  for(;;)
  {
    https_cnt++;

    if(https_cnt >= 5)
    {
      https_cnt = 0;
      tg_update_id(&next_id, TOKEN, rec_buff, sizeof(rec_buff));
      if(next_id > st_id)
      {
        st_id = next_id;

        pfind = strstr (rec_buff, "text\":\"");
        if(NULL != pfind)
        {
          //some work
          tg_send_msg("Done", sizeof("Done"), TOKEN, CHAT_ID);
        }
      }
    }
    osDelay(1000);

  }
}




void Error_Handler(void)       //
{
  __disable_irq();
  while (1);
}


