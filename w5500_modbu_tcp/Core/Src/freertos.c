

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "socket.h"
#include "server.h"
#include "mb.h"
#include "mbtcp.h"


/* freeMODBUS */
USHORT   usRegInputStart = REG_INPUT_START;
SHORT    usRegInputBuf[REG_INPUT_NREGS] = {0};

USHORT   usRegHoldingStart = REG_HOLDING_START;
SHORT    usRegHoldingBuf[REG_HOLDING_NREGS] = {0};
/* freeMODBUS */


void SystemClock_Config(void);

/* externs */
extern wiz_PhyConf phy_param;  //speed/mode
extern wiz_NetInfo net_param;  //set param
extern wiz_NetInfo net_param_read; 
/* externs */


#if ( DEBUG == 1)
/* debug values */
static size_t free_size = 0;
extern volatile long isr_test;  //for debug
/* debug values */
#endif

typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;


/* Definitions for tcpCountingSem */
osSemaphoreId_t Sem_tcp_reciv_sem;
osStaticSemaphoreDef_t myCountingSem01ControlBlock;
const osSemaphoreAttr_t myCountingSem01_attributes = {
  .name = "tcpCountingSem",
  .cb_mem = &myCountingSem01ControlBlock,
  .cb_size = sizeof(myCountingSem01ControlBlock),
};



/* Definitions for ModBusTask */
osThreadId_t ModBusTaskHandle;
uint32_t ModBusTaskBuffer[ 1024 ];
osStaticThreadDef_t ModBusTaskControlBlock;
const osThreadAttr_t ModBusTask_attributes = {
  .name = "ModBusTask",
  .cb_mem = &ModBusTaskControlBlock,
  .cb_size = sizeof(ModBusTaskControlBlock),
  .stack_mem = &ModBusTaskBuffer[0],
  .stack_size = sizeof(ModBusTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal1,
};

/* Definitions for Lcd_Bottons_tas */
osThreadId_t Lcd_Bottons_tasHandle;
uint32_t Lcd_Bottons_tasBuffer[ 256 ];
osStaticThreadDef_t Lcd_Bottons_tasControlBlock;
const osThreadAttr_t Lcd_Bottons_tas_attributes = {
  .name = "Lcd_Bottons_tas",
  .cb_mem = &Lcd_Bottons_tasControlBlock,
  .cb_size = sizeof(Lcd_Bottons_tasControlBlock),
  .stack_mem = &Lcd_Bottons_tasBuffer[0],
  .stack_size = sizeof(Lcd_Bottons_tasBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Logic_task */
osThreadId_t Logic_taskHandle;
uint32_t Logic_taskBuffer[ 256 ];
osStaticThreadDef_t Logic_taskControlBlock;
const osThreadAttr_t Logic_task_attributes = {
  .name = "Logic_task",
  .cb_mem = &Logic_taskControlBlock,
  .cb_size = sizeof(Logic_taskControlBlock),
  .stack_mem = &Logic_taskBuffer[0],
  .stack_size = sizeof(Logic_taskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Calcul_task */
osThreadId_t Calcul_taskHandle;
uint32_t Calcul_taskBuffer[ 256 ];
osStaticThreadDef_t Calcul_taskControlBlock;
const osThreadAttr_t Calcul_task_attributes = {
  .name = "Calcul_task",
  .cb_mem = &Calcul_taskControlBlock,
  .cb_size = sizeof(Calcul_taskControlBlock),
  .stack_mem = &Calcul_taskBuffer[0],
  .stack_size = sizeof(Calcul_taskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};



void modbus_task(void *argument);
void lcd_bottons_task(void *argument);
void logic_task(void *argument);
void calcul_task(void *argument);

void MX_FREERTOS_Init(void); 


/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook (void);
void vApplicationIdleHook(void);



void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  while(1);
}


void vApplicationMallocFailedHook (void)
{
  while(1);
}


void vApplicationIdleHook( void )
{
}

void MX_FREERTOS_Init(void) 
{
  
  /* add mutexes, ... */
  /* add semaphores, ... */
  Sem_tcp_reciv_sem = osSemaphoreNew(10000, 0, &myCountingSem01_attributes);
  /* start timers, add new ones, ... */
  /* add queues, ... */
  
  /* Create the thread(s) */
  /* creation of ModBusTask */
  ModBusTaskHandle = osThreadNew(modbus_task, NULL, &ModBusTask_attributes);
  
  /* creation of Lcd_Bottons_tas */
  Lcd_Bottons_tasHandle = osThreadNew(lcd_bottons_task, NULL, &Lcd_Bottons_tas_attributes);
  
  /* creation of Logic_task */
  Logic_taskHandle = osThreadNew(logic_task, NULL, &Logic_task_attributes);
  
  /* creation of Calcul_task */
  Calcul_taskHandle = osThreadNew(calcul_task, NULL, &Calcul_task_attributes);
  
  
}


void modbus_task(void *argument)
{
  
  int res = 0;
  static uint8_t sock_st[MAX_SOCK] = { 0 };
  int8_t link_stat = -1;
  eMBErrorCode eModBusState = MB_ENOERR;
  
  free_size = xPortGetFreeHeapSize(); 
  
  eModBusState = eMBTCPInit(configPORT);
  
  if(MB_ENOERR == eModBusState)
   eModBusState = eMBEnable();
  

  if(IP_NOT_SET || MASK_NOT_SET || GW_NOT_SET)
  { 
    while(IP_NOT_SET || MASK_NOT_SET || GW_NOT_SET) 
      link_stat = w5500_full_reset(); 
  }
  
  for(;;)
  {
    
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin); //DEBUG
    
    link_stat = wizphy_getphylink();
    wizchip_getnetinfo(&net_param_read);
    
    if(IP_NOT_SET || MASK_NOT_SET || GW_NOT_SET)
    {
      while(IP_NOT_SET || MASK_NOT_SET || GW_NOT_SET)
      {
        link_stat = w5500_full_reset(); 
        osDelay(100);
      }
    }
    
    if(link_stat)
    {
      for(uint8_t sock = 0; sock < 8; sock++)
      {
        sock_st[sock] = tcp_ip_ser_run(sock);
        if(sock_st[sock] == SOCK_LISTEN ||\
          sock_st[sock] == SOCK_ESTABLISHED )
          res++;
      }
      
      /*  if all socket is listening or established we'll wait recv
      interrupt from int pin which will give us semaphore*/
      
      if( res == MAX_SOCK ) 
      { 
        res = 0; //
        if(0 == osSemaphoreGetCount(Sem_tcp_reciv_sem)) //recv interrupt is't
        {
          for(uint8_t sock = 0; sock < 8; sock++)
          {
            if(getSn_IR(sock) & Sn_IR_RECV) 
              res++; 
          }
          if(!res)
            osSemaphoreAcquire(Sem_tcp_reciv_sem, portMAX_DELAY);//wait untill recv int/
          res = 0;
        }
        else  //some response is lost
        {
          while (0 != osSemaphoreGetCount(Sem_tcp_reciv_sem)) 
            osSemaphoreAcquire(Sem_tcp_reciv_sem, 0);   //clear semaphore
        }
      }
      else
      {
        res = 0;
        osThreadYield();
      }
      
      //DEBUG
      usRegInputBuf[0]+=1;
      usRegInputBuf[1]+=2;
      usRegInputBuf[2]+=3;
      usRegInputBuf[3]+=4;
      usRegInputBuf[4]+=5;
      //DEBUG
    }
    else //Ethernet is't attached 
      osThreadYield();
    
  }
}


void lcd_bottons_task(void *argument)
{
  for(;;)
  {
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD3_Pin);
    osDelay(500);
  }
}


void logic_task(void *argument)
{
  for(;;)
  {
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD5_Pin);
    osDelay(100);
  }
  
}


void calcul_task(void *argument)
{
  for(;;)
  {
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD6_Pin);
    osDelay(1000);
  }
}




eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  
  if( ( usAddress >= REG_INPUT_START )
     && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegInputStart );
    while( usNRegs > 0 )
    {
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
      iRegIndex++;
      usNRegs--;
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  
  return eStatus;
}


eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  
  int iRegIndex;
  
  if( ( usAddress >= REG_HOLDING_START ) &&
     ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    usAddress++;
    iRegIndex = ( int )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
      
    case MB_REG_READ:
      while( usNRegs > 0 )
      { 
        *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
        *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );  
        iRegIndex++;
        usNRegs--;
      }
      break;
      
    case MB_REG_WRITE:
      while( usNRegs > 0 )
      {
        
        usRegHoldingBuf[iRegIndex]  = *pucRegBuffer++ << 8;
        usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;  
        
        iRegIndex++;
        usNRegs--;        
      }
      
      break;
    }
    
    
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
  
}



eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
  return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  return MB_ENOREG;
}




