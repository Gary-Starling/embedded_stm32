#include "sim76xx.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "RingBuff.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
23.05.23
Пофиксен баг с отправкой UART_DMA, добавлена проверка TC uart, добалено ожидание gState.
Включено прерывание UART_IT_TC
*/

/* структуры для работы с uart rx/tx dma*/
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* Блокировка uart */
extern osMutexId_t MutexSim76xxHandle;
/* Очередь от парсера, ответы на запросы sim_send */
extern osMessageQueueId_t SimAnsSendQueueHandle;
/* Очредь от парсера, внеочерденые данные, Ring, Msg... */
extern osMessageQueueId_t SimExtraMsgQueueHandle;

/* Структура с которой работает dma/uart (from RingBuff.h)*/
sRingBuff sim_buff;

static void sim76xx_mng_pin_init(void);
static void UART3_Init(void);
static void UART3_DeInit(void);
/*
Power off
The following methods can be used to power off module.
Method 1: Power off module by pulling the PWRKEY pin down to ground.
Method 2: Power off module by AT command “AT+CPOF”.
Method 3: over-voltage or under-voltage automatic power off. The voltage
range can be set by AT command “AT+CPMVT”.
Method 4: over-temperature or under-temperature automatic power off.
*/


#if SIM_DEBUG == 1
#endif


void init_uart(void)
{
  UART3_Init();
}

void deinit_uart(void)
{
  UART3_DeInit();
}

/**
Инит UART3 RX/TX
Включение прерываний IDLE, PARITY, OVERRUN, NOISE
*/
static void UART3_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_USART3_CLK_ENABLE();

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);

  USART3->CR1 |= USART_CR1_IDLEIE;      //UART3 IDLE Interrupt Configuration
  USART3->CR1 |= USART_CR1_PEIE;        //Parity error
  USART3->CR3 |= USART_CR3_EIE;         //error, overrun error or noise flag
  USART3->CR1 |= UART_IT_TC;            //transmittion cmpt

  HAL_NVIC_EnableIRQ(USART3_IRQn);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

  RingRxDmaStart(&sim_buff);        //
}


static void UART3_DeInit(void)
{
  huart3.gState =  HAL_UART_STATE_RESET;
  USART3->CR1 &= ~USART_CR1_IDLEIE;      //UART3 IDLE Interrupt Configuration
  USART3->CR1 &= ~USART_CR1_PEIE;        //Parity error
  USART3->CR3 &= ~USART_CR3_EIE;         //error, overrun error or noise flag
  USART3->CR1 &= ~UART_IT_TC;            //transmittion cmpt

  HAL_UART_MspDeInit(&huart3);

  HAL_NVIC_DisableIRQ(USART3_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream1_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream3_IRQn);

  RingRxDmaStop();

  __HAL_RCC_DMA1_CLK_DISABLE();
}

/**
Инит пинов используемых для включения/выключения и перезапуска модуля
*/
static void sim76xx_mng_pin_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GSM_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GSM_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PWR_KEY_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(PWR_KEY_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RESET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(RESET_PORT, &GPIO_InitStruct);
}



/**
Включение модуля и связанного с ним периферией
return eComRes - возращает результат инициализации
*/
eComRes sim76xx_init(void)
{
  eComRes res;

  init_uart();                      //Общение с модулем
  sim76xx_mng_pin_init();           //Это пины мк


  GSM_OFF;
#ifdef SIM_RTOS
  osDelay(100);
#elif
  HAL_Delay(100);
#endif
  GSM_ON;

  /* pin to power on module Ton typ. 500ms */
  PWR_KEY_OFF;
#ifdef SIM_RTOS
  osDelay(500);
#elif
  HAL_Delay(500);
#endif
  PWR_KEY_ON;

  /* NOTE The time from power-on issue to UART port ready 12 sec */
  /* The time from power-on issue to STATUS pin output typ 16 sec */
  if( (res = sim76xx_pwr_up_wait()) != AT_SEND_OK)
    return res;
  /* Наши настройки */
  if( (res = sim76xx_set_settings()) != AT_SEND_OK)
    return res;
  /* Настройка пинов упралямых модулем */
  if( (res = sim76xx_gpio_init()) != AT_SEND_OK)
    return res;
  /* Интернет соединение */
  if( (res = sim76xx_internet_init())!= AT_SEND_OK)
    return res;

  return AT_SEND_OK; //ready to work
}


/**
  Ожидание включения
  noparam
  return eComRes - возращает результат инициализации
*/
eComRes sim76xx_pwr_up_wait(void)
{
  eComRes res;
  if((res = sim76xx_send_at_com(WAIT_STATE, "RDY", NULL, 15000)) != AT_SEND_OK)
    return res;

  if((res = sim76xx_send_at_com(WAIT_STATE, "+CPIN: READY", NULL, 5000)) != AT_SEND_OK)
    return res;

  if((res = sim76xx_send_at_com(WAIT_STATE, "SMS DONE", NULL, 5000)) != AT_SEND_OK)
    return res;

  if((res = sim76xx_send_at_com(WAIT_STATE, "PB DONE", NULL, 5000)) != AT_SEND_OK)
    return res;

  //Ну удаляй эту задержку, она нужна при инициализации
  //После @PB DONE@ должно пройти достаточно много времени
#ifdef SIM_RTOS
  osDelay(30000);
#elif
  HAL_Delay(30000);
#endif
  return AT_SEND_OK;
}


/**
  Инициализация пинов sim76xx
  noparam
  return eComRes - возращает результат инициализации
*/
eComRes sim76xx_gpio_init(void)
{
  eComRes res;
  const char * pins[10] = {PIN1_GPIO, PIN2_GPIO, PIN3_GPIO, PIN4_GPIO, PIN5_GPIO,
  VD1_OUT, VD2_OUT, VD3_OUT, VD4_OUT, VD5_OUT};

  //pin's out settings

  for(size_t i = 0; i < 10; i++)
  {
    if((res = sim76xx_send_at_com(pins[i], "OK", NULL, 10000)) != AT_SEND_OK)
      return res;
  }

  return AT_SEND_OK;
}

/**
  Функция мигания светодиодами от кастомной платы.
  uin32_t time - как часто мигать в мсек
  noreturn
*/
void sim76xx_led_blink(uint32_t time, uint8_t mode)
{
  const char * leds_st[10] = {VD1_SET, VD1_RESET, VD2_SET, VD2_RESET, VD3_SET,
  VD3_RESET, VD4_SET, VD4_RESET, VD5_SET, VD5_RESET };
  size_t i;

  if(mode == 0)
  {
    for(i = 0; i < 10; i+=2)
    {
      sim76xx_send_at_com(leds_st[i], "OK", NULL, 3000);
#ifdef SIM_RTOS
      osDelay(time);
#elif
      HAL_Delay(time);
#endif
    }
    for(i = 1; i < 10; i+=2)
    {
      sim76xx_send_at_com(leds_st[i], "OK", NULL, 3000);
#ifdef SIM_RTOS
      osDelay(time);
#elif
      HAL_Delay(time);
#endif
    }
  }
  else
  {
    for(i = 0; i < 10; i++)
    {
      sim76xx_send_at_com(leds_st[i], "OK", NULL, 3000);
#ifdef SIM_RTOS
      osDelay(time);
#elif
      HAL_Delay(time);
#endif
      sim76xx_send_at_com(leds_st[i], "OK", NULL, 3000);
#ifdef SIM_RTOS
      osDelay(time);
#elif
      HAL_Delay(time);
#endif
    }
  }
}

/**
  Функция ответа на входящий вызов, также убирает настройки шумоподавления, т.к.
  на вход i2s подаётся предварительно записанные данные.
  noparam
  return eComRes - возращает результат инициализации
*/
eComRes sim76xx_answ_call(void)
{
  eComRes res;

  if( (res = sim76xx_send_at_com(ANSWR_CALL, "VOICE CALL: BEGIN", NULL, 9000)) != AT_SEND_OK )
    return res;
  if( (res = sim76xx_send_at_com(WAIT_STATE, "OK", NULL, 1000)) != AT_SEND_OK )
    return res;
  /* Убираем настройки шумоподавления
  if( (res = sim76xx_send_at_com("AT+CNSN=0x0000\r\n", "OK", 3000)) != AT_SEND_OK )
  return res;
  if( (res = sim76xx_send_at_com("AT+CNSLIM=0x000\r\n", "OK", 3000)) != AT_SEND_OK )
  return res;
  if( (res = sim76xx_send_at_com("AT+CECH=0x0000\r\n", "OK", 3000)) != AT_SEND_OK )
  return res;
  if( (res = sim76xx_send_at_com("AT+CECDT=0x0000\r\n", "OK", 3000)) != AT_SEND_OK )
  return res;
  if( (res = sim76xx_send_at_com("AT+CECWB=0x0000\r\n", "OK", 3000)) != AT_SEND_OK )
  return res;
  if( (res = sim76xx_send_at_com("AT+CECM=1\r\n", "OK", 3000)) != AT_SEND_OK )
  return res;
  if( (res = sim76xx_send_at_com(SIDET_DI, "OK", 3000)) != AT_SEND_OK )
  return res;
  */
  return AT_SEND_OK;
}

/**
  Настройки модуля sim76xx
  noparam
  return eComRes - возращает результат инициализации
*/
eComRes sim76xx_set_settings(void)
{
  eComRes res;

  //baudrate
  if( (res = sim76xx_send_at_com(BAUDR_115200, "OK", NULL, 15000)) != AT_SEND_OK )
    return res;
  //echo disable
  if( (res = sim76xx_send_at_com(ECHO_OFF, "OK", NULL, 10000)) != AT_SEND_OK )
    return res;
  //req func look at terminal
  if((res = sim76xx_send_at_com(REQUEST_FUNC, "+CFUN", NULL, 10000)) != AT_SEND_OK)
    return res;
  //set full func
  if((res = sim76xx_send_at_com(SET_FULL_FUNC, "OK", NULL, 10000)) != AT_SEND_OK)
    return res;
  //pcm 8khz
  if((res = sim76xx_send_at_com(PCM_RATE_8KHZ, "OK", NULL, 10000)) != AT_SEND_OK)
    return res;
  if((res = sim76xx_send_at_com(PCM_RATE, "OK", NULL, 10000)) != AT_SEND_OK)
    return res;
  //dtmf
  if((res = sim76xx_send_at_com(DTMF_TONE_INST, "+VTD: ", NULL, 10000)) != AT_SEND_OK)
    return res;
  //sms(delete all)
  if((res = sim76xx_send_at_com(DEL_ALL_SMS, "OK", NULL, 10000)) != AT_SEND_OK)
    return res;
  if((res = sim76xx_send_at_com(SMS_FORMAT_INST, "+CMGF:", NULL, 10000)) != AT_SEND_OK)
    return res;
  if((res = sim76xx_send_at_com(SMS_FORMAT_TXT, "OK", NULL, 10000)) != AT_SEND_OK)
    return res;
  if((res = sim76xx_send_at_com("AT+CPMS=?\r\n", "+CPMS:", NULL, 10000)) != AT_SEND_OK)
    return res;
  if((res = sim76xx_send_at_com("AT+CPMS?\r\n", "+CPMS:", NULL, 10000)) != AT_SEND_OK)
    return res;
  //FLASH message storage
  if((res = sim76xx_send_at_com("AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n", "+CPMS:", NULL, 10000)) != AT_SEND_OK)
    return res;
  if((res = sim76xx_send_at_com(DEL_ALL_SMS, "OK", NULL, 10000)) != AT_SEND_OK)
    return res;
  if((res = sim76xx_send_at_com("AT+CSCS=\"UCS2\"\r\n", "OK", NULL, 10000)) != AT_SEND_OK)
    return res;

  return AT_SEND_OK;
}

/* Активация internet соединения */
eComRes sim76xx_internet_init(void)
{
  eComRes res;
  if((res = sim76xx_send_at_com(READ_ICCID, "OK", NULL, 3000)) != AT_SEND_OK)
    return res;

  //TODO: Ttimeout
  //Регистрация в сети
  while(1)
  {
    if(sim76xx_send_at_com(LIST_CREG, "+CREG: 0,1", NULL, 1000) == AT_SEND_OK)
      break;
    if(sim76xx_send_at_com(LIST_CREG, "+CREG: 1,1", NULL, 1000) == AT_SEND_OK)
      break;
    if(sim76xx_send_at_com(LIST_CREG, "+CREG: 2,1", NULL, 1000) == AT_SEND_OK)
      break;
  }

  //  if((res = sim76xx_send_at_com(LIST_MODE, "+CNMP:", NULL, 3000)) != AT_SEND_OK)
  //    return res;

  if((res = sim76xx_send_at_com(GPRS_ENABLE, "OK", NULL, 3000)) != AT_SEND_OK)
    return res;

  if((res = sim76xx_send_at_com(PDP_ENABLE, "OK", NULL, 3000)) != AT_SEND_OK)
    return res;

  if((res = sim76xx_send_at_com(APN_MEGAFON, "OK", NULL, 3000)) != AT_SEND_OK)
    return res;

  if((res = sim76xx_send_at_com(IP_ADDR, "+CGPADDR:", NULL, 3000)) != AT_SEND_OK)
    return res;

  return AT_SEND_OK;
}


/**
*/
void sim76xx_reset(void)
{
  /* The active low level time impulse on RESET pin to
  reset module min = 100 typ = 200 max = 500 ms  */
  RESET_ON;
#ifdef SIM_RTOS
  osDelay(500);
#elif
  HAL_Delay(500);
#endif
  RESET_OFF;
}



/**
  Парсер входящих сообщений.
  Отправляет ответ в sim76xx_send_at_com, если это не экстра сообщение.
  При экстра сообщении, отправляет его задаче обработчику.
  Списко экстра сообщений смотри в sim76xx.h.

  void * rec_buff - буфер, куда складываем данные
  noreturn
*/

#pragma optimize=none
void sim76xx_parsing(void * rec_buff)
{
  const char * extra_msg[MAX_EXTRA_MES] = {
    "RING", "VOICE CALL: END:", "MISSED_CALL", "+RXDTMF: ", "+CMTI: \"ME\"", "+SMS FULL",
    "NO CARRIER", "+SIMCARD: NOT AVAILABLE", "+CME ERROR: SIM not inserted", "Error Init" };
  sAnswrObject copy = {0};    //мы копируем в очередь, так что это работает
  size_t i = 0;               //для чтения буфера
  osStatus_t osResMsg = osOK; //для отладки TODO:обработка ошибки
  sExtraMsg Extra;            //

  if(rec_buff == NULL) return;

  //Мы должны читать быстрее чем принимаем данные, скопируем строку
  //- 2 т.к. нам нужно добавить \0
  while( UnreadDataBuff((sRingBuff *)rec_buff) && (i < ANSWER_SIZE - 2) )
    copy.buff[i++] = ReadRingData((sRingBuff *)rec_buff);
  copy.buff[i] = '\0';

  if(i < 4) return; // min answrer OK\r\n

  /* Проверка на экстра сообщение */
  for(size_t j = 0; j < MAX_EXTRA_MES; j++)
  {
    if(strstr( (const char *)(copy.buff), extra_msg[j] ) )
    {
      Extra.msg = (eMsgExtra)j;
      switch(j)
      {
        case SMS:
        Extra.val = (char)atoi((char*)&copy.buff[14]); //Номер смс в памяти
        break;

        case RXDTMF:
        Extra.val = copy.buff[12];                      //Символ dtmf
        break;

      }
      osResMsg = osMessageQueuePut (SimExtraMsgQueueHandle, &Extra, 0, 0); //
      return;
    }
  }

  /* Не нашли экстра сообщений, значит ответ на команду */
  osResMsg = osMessageQueuePut (SimAnsSendQueueHandle, &copy, 0, 0); //сигналим отправителю
}



/**
  Функция отправки команды модулю.
  const char * com - отправляемая команда
  const char * ans - предпологаемый ответ
  const char * pSMS - указатель для копирования ответа, если нужно
  uint8_t max_t - максимальное время ответа в мСек
  return eComRes - результат выполнения операции:
*/
eComRes sim76xx_send_at_com(const char * com,
                            const char * wait_ans,
                            char * const copy,
                            size_t max_t)
{
  extern DMA_HandleTypeDef hdma_usart3_tx;
  osStatus_t osMutSt = osOK;    //Результат работы с mutex
  osStatus_t osMesSt = osOK;    //Результат работы с queue
  sAnswrObject rec_answer;      //Принятый ответ
  HAL_StatusTypeDef st;

  if(wait_ans == NULL || max_t == 0) return AT_ERR_PARAM;

  /* захват uart */
  osMutSt = osMutexAcquire (MutexSim76xxHandle,  osWaitForever);

  if(osMutSt != osOK)
    return AT_MQ_ERROR;                 //Ошибка mutex

  /* Отправка сообщения в sim76xx.
  Если мы  просто ждём какого-либо ответа  то ------------------------------------------|*////|
  if(strstr((const char *)"WAIT STATE", com) == NULL)                                       //|
  {
    while((USART3->SR & USART_SR_TC) == 0);
    //TODO:timeoit
    DMA1_Stream3->CR &= (uint32_t)(~DMA_SxCR_DBM);
    /* Configure DMA Stream data length */
    DMA1_Stream3->NDTR = strlen(com);
      /* Configure DMA Stream destination address */
     DMA1_Stream3->PAR = (uint32_t)&USART3->DR;
    /* Configure DMA Stream source address */
    DMA1_Stream3->M0AR = (uint32_t)com;
    /*clear intrupts*/
    *(uint32_t *)(DMA1_BASE + 0x0008) = 0x3FU << 22;
    /* Enable Common interrupts*/
    DMA1_Stream3->CR  |= DMA_IT_TE | DMA_IT_DME | DMA_IT_TC;
    DMA1_Stream3->CR |=  DMA_SxCR_EN;
    USART3->SR = ~(UART_FLAG_TC);
    USART3->CR3 |= USART_CR3_DMAT;
  }                                                                                         //|
                                                                                            //|
  /* Встаём в ожидание от парсера, мы можем получить сообщение, выйти по                    //|
  таймауту, или же ошибка сообщения */                                                      //|
  osMesSt = osMessageQueueGet(SimAnsSendQueueHandle, &rec_answer, NULL, max_t);// <-----|/////|

  /* Нас интересует ОК и timeout, остальное, ошибки ресурсов и т.п.*/
  if(osMesSt != osOK)
  {
    osMutexRelease (MutexSim76xxHandle);               //Отдадим ресурс
    if(osMesSt == osErrorTimeout)
      return AT_TIME_OUT;  //Не дождались сообщения
    else
      return AT_MQ_ERROR;                           //Ошибка очереди
  }

  /* Приняли ответ от команды */
  if(strstr((const char *)rec_answer.buff, wait_ans) == NULL)
  {
    /* Строки не совпали */
    osMutexRelease (MutexSim76xxHandle);
    return AT_WR_ANSWER;
  }

  /* Если нужно скопировать входящий ответ */
  //TODO: Проверка соответствия размеров
  if(copy != NULL)
    memcpy(copy, (const void *)rec_answer.buff, strlen(rec_answer.buff) + 1); //+\0

  osMutexRelease (MutexSim76xxHandle);

  return  AT_SEND_OK;
}



/**
    TODO:Test
    Софтовый перезапуск.
    noparam
    noreturn
*/
void soft_sim76xx_reset(void)
{
  sim76xx_send_at_com(PIN1_GPIO, "OK", NULL, 9000);
}


/**
  Функция перезапуска.
  Если модуль перестаёт отвечать по uart, используй её, через 0.5 сек., модуль
  начнёт отвечать.
  noparam
  noreturn
*/
void sim76xx_uart_reset(void)
{

  deinit_uart();
  init_uart();
  if(sim76xx_send_at_com("AT\r\n", "OK", NULL, 1000))
    sim76xx_reset();
}

/* USART3 IRQ */
void USART3_IRQHandler(void)
{
  extern osSemaphoreId_t sim76xxSemHandle;
  uint8_t err = 0;

  /* UART IDLE Interrupt */
  if( (USART3->SR & USART_SR_IDLE) != RESET )
  {
    /* It is cleared by a software sequence */
    (void)USART3->SR;
    (void)USART3->DR;

    PutRingData(&sim_buff);

    /* last idle, we have msg
    '\n' - end msg or ' ' - it's >' '*/
    if(*sim_buff.prvt == '\n' || *sim_buff.prvt == ' ')
      osSemaphoreRelease(sim76xxSemHandle); //to Sim76xxParser task

  }

  //TODO::all errors
  //HAL_UART_IRQHandler(&huart3);


  /* parity error */
  if ( (USART3->SR & USART_SR_PE) != RESET )
    err++;

  /* noise error */
  if ( (USART3->SR & USART_SR_NE) != RESET )
    err++;

  /* frame error */
  if ( (USART3->SR & USART_SR_FE) != RESET )
    err++;

  /* Over-Run */
  if ( (USART3->SR & USART_SR_ORE) != RESET )
      err++;

  /* sone error */
  if (err)
  {
    (void)USART3->SR;
    (void)USART3->DR;

    if ( (USART3->SR & USART_SR_ORE) != RESET )
    {
      USART3->CR1 &= ~USART_CR1_IDLEIE;      //UART3 IDLE Interrupt Configuration
      USART3->CR1 &= ~USART_CR1_PEIE;        //Parity error
      USART3->CR3 &= ~USART_CR3_EIE;         //error, overrun error or noise flag
      USART3->CR1 &= ~UART_IT_TC;            //transmittion cmpt
      USART3->CR3 &= ~USART_CR3_DMAR;
    }
  }

  /* Transmitter end */
  if ( ((USART3->SR & USART_SR_TC) != RESET) && ((USART3->CR1 & USART_CR1_TCIE) != RESET))
      USART3->CR1 &= ~UART_IT_TC;

}


void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
}


void DMA1_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
}