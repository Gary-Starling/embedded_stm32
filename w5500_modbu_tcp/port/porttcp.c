/*

*/

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <string.h>
#include <portm.h>
#include <spi.h>

/* ----------------------- w5500 includes------------------------------------*/
#include <socket.h>
#include <server.h>

/* ----------------------- global variables -----------------------------------*/
uint8_t ucTCPRequestFrame[configMAX_SIZE];  // recive buff
uint8_t ucTCPResponseFrame[configMAX_SIZE]; // send buff
uint16_t usTCPResponseLen;                   
uint16_t usTCPRequestLen;
uint8_t ucFrameSent = FALSE; // Отправлять ли ответное решение

/* ----------------------- w5500 externs ------------------------------------*/
extern volatile long isr_test; 
extern wiz_PhyConf phy_param;  //speed/mode
extern wiz_NetInfo net_param;  //set param
extern wiz_NetInfo net_param_read; 

/* ----------------------- Defines  -----------------------------------------*/


/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_UID          6
#define MB_TCP_LEN          4
#define MB_TCP_FUNC         7
/* ----------------------- Prototypes ---------------------------------------*/
void            vMBPortEventClose( void );
void            vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule,
                           const CHAR * szFmt, ... );

/* ----------------------- Static variables ---------------------------------*/
/* ----------------------- Static functions ---------------------------------*/
/* ----------------------- Begin implementation -----------------------------*/
BOOL
xMBTCPPortInit( USHORT usTCPPort )
{
  BOOL stat = FALSE;
  
  stat = (MX_SPI1_Init() == HAL_OK)?  TRUE : FALSE;
  
  if(stat) 
  {
    w5500_init();
    wizphy_reset();
    wizphy_setphyconf(&phy_param);
    wizphy_getphystat(&phy_param);
    wizchip_getnetinfo(&net_param_read); 
  }
  else
  {
    /* error spi*/
    HAL_SPI1_deInit();
    return FALSE;
  }
    
  
  if(IP_NOT_SET || MASK_NOT_SET || GW_NOT_SET)
    return FALSE;
  else
    return TRUE;
}


void
vMBTCPPortClose(  )
{
  
}

void
vMBTCPPortDisable( void )
{
  
}


BOOL
xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
  *ppucMBTCPFrame = (uint8_t *) &ucTCPRequestFrame[0];  
  *usTCPLength = usTCPRequestLen;    
  usTCPRequestLen = 0; //reset buffer 
  return TRUE;  
}

BOOL
xMBTCPPortSendResponse( const UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
  
  memcpy(ucTCPResponseFrame,pucMBTCPFrame , usTCPLength);  
  usTCPResponseLen = usTCPLength;  
  ucFrameSent = TRUE; // send data through W5500
  return ucFrameSent;
}
