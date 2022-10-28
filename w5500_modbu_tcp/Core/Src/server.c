#include <string.h>
#include <stdio.h>

#include "stm32f4xx_hal.h" 
#include "server.h"
#include "socket.h"
#include "spi.h"
#include "cmsis_os.h"
#include "mbport.h"
#include "mb.h"


wiz_PhyConf phy_param = {
  .by = PHY_CONFBY_SW,       // PHY_CONFBY_SW
  .mode = PHY_MODE_AUTONEGO, // PHY_MODE_AUTONEGO
  .speed = PHY_DUPLEX_FULL,  // PHY_SPEED_100
  .duplex = PHY_DUPLEX_FULL  // PHY_DUPLEX_FULL
};


wiz_NetInfo net_param = { 
  .mac = {0x02, 0x04, 0x08, 0x0A, 0x1A, 0x2A},       ///< Source Mac Address (Пока любой)
  .ip =  {192, 168, 1, 50},                          ///< Source IP Address
  .sn =  {255, 255, 255, 0},                         ///< Subnet Mask (Маска подсети)
  .gw =  {192, 168, 1, 1},                           ///< Gateway IP Address(Шлюз)
  .dns = {0, 0, 0, 0},                               ///< DNS server IP Address(Пока не нужнен выставим 0000)
  .dhcp = NETINFO_STATIC                             ///< 1 - Static, 2 - DHCP (Ставим руками ip адрес)
};

wiz_NetInfo net_param_read = {0};

extern  uint8_t ucTCPRequestFrame[configMAX_SIZE];  // recive buff
extern uint8_t ucTCPResponseFrame[configPORT]; 	    // send buff
extern uint16_t usTCPResponseLen;                   
extern uint16_t usTCPRequestLen;
extern uint8_t ucFrameSent; 


extern osSemaphoreId_t Sem_tcp_reciv_sem;


void w5500_sel(void) 
{
  HAL_GPIO_WritePin(CS_SPI_GPIO_Port, CS_SPI_Pin, GPIO_PIN_RESET);
}

void w5500_unsel(void) 
{
  HAL_GPIO_WritePin(CS_SPI_GPIO_Port, CS_SPI_Pin, GPIO_PIN_SET);
}

void w5500_read_buff(uint8_t* buff, uint16_t len) 
{
  HAL_SPI_Receive(&hspi1, buff, len, HAL_MAX_DELAY);
}

void w5500_write_buff(uint8_t* buff, uint16_t len) 
{
  HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

uint8_t w5500_read_byte(void) 
{
  uint8_t byte;
  
  w5500_read_buff(&byte, sizeof(byte));
  return byte;
}

void w5500_write_byte(uint8_t byte)
{
  w5500_write_buff(&byte, sizeof(byte));
}



void w5500_enter_critical(void)
{
  taskENTER_CRITICAL(); 
}

void w5500_exit_critical(void)
{
  taskEXIT_CRITICAL();	
}

void w5500_init(void)
{
  /* buff size*/
  uint8_t rx_tx_buff_size[8] = {2, 2, 2, 2, 2, 2, 2, 2};
  
  /* deinit - init w5500 */
  HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
  osDelay(1);         // >= 500uSec
  HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);
  osDelay(15);        // >= 10mSec
  
  /* reg callbacks wizchip_conf.c/.h */ 
  reg_wizchip_cris_cbfunc(w5500_enter_critical, w5500_exit_critical);   //critical reg callback
  reg_wizchip_cs_cbfunc(w5500_sel, w5500_unsel);                        //CS reg callback
  reg_wizchip_spi_cbfunc(w5500_read_byte, w5500_write_byte);            //read/write 1 byte
  reg_wizchip_spiburst_cbfunc(w5500_read_buff, w5500_write_buff);       //read/write buff
  wizchip_init(rx_tx_buff_size, rx_tx_buff_size);                       //init buff read and write size
  wizchip_setnetinfo(&net_param);                                       //set net param
  ctlnetwork(CN_SET_NETINFO, (void*) &net_param);                      
  
  /* mask RECV interrupt for all sock */
  for(uint8_t sock_n = 0;  sock_n < 8; sock_n++)
    setSn_IMR(sock_n, RECV_IR_MSK);
  /* enable inerrupts */
  setSIMR(0xff);  
}


uint8_t tcp_ip_ser_run(uint8_t sock_number)
{
  
  uint8_t sock_sn = 0;
  
  sock_sn = getSn_SR(sock_number);
  
  switch(sock_sn)
  {
    //==============================================================================    
  case SOCK_ESTABLISHED:
    /* Interrupt clear */
    if(getSn_IR(sock_number) & Sn_IR_RECV)      //recv interrupt
    {
      setSn_IR(sock_number, Sn_IR_RECV);        // clear Sn_IR_RECV
      osSemaphoreAcquire(Sem_tcp_reciv_sem, 0); // clear semaphore from EXTI9_5_IRQHandler 
    }
    if(getSn_IR(sock_number) & Sn_IR_CON)
      setSn_IR(sock_number, Sn_IR_CON);
    /* Interrupt clear */
    
    if ((usTCPRequestLen = getSn_RX_RSR(sock_number)) > 0)
    {
      if (usTCPRequestLen > configMAX_SIZE) usTCPRequestLen = configMAX_SIZE;
      
      usTCPRequestLen = recv(sock_number, ucTCPRequestFrame, usTCPRequestLen);
      xMBPortEventPost (EV_FRAME_RECEIVED); //some data recived, send EV_FRAME_RECEIVED
      eMBPoll (); //  EV_FRAME_RECEIVED handle, switch to EV_EXECUTE
      eMBPoll (); //  EV_EXECUTE handle
      
      if(ucFrameSent)  
      {  
        ucFrameSent = FALSE;  
        send(sock_number, ucTCPResponseFrame, usTCPResponseLen);  // W5500 is sending pack of data to Modbus
        xMBPortEventPost (EV_FRAME_SENT); //some data recived, send EV_FRAME_RECEIVED
        eMBPoll (); // EV_FRAME_SENT (i'm thinking it's too much) 
      }  
    }
    break;
    //==========================================================================    
  case SOCK_TIME_WAIT:
    break;
    //==========================================================================    
  case SOCK_CLOSE_WAIT:
    disconnect(sock_number);
    break;
    //==========================================================================
  case SOCK_CLOSED: 
    if(socket(sock_number, Sn_MR_TCP, configPORT, 0x00) == SOCKERR_SOCKINIT)
    {
      /* need claer semaphore or we don't start wait on task modbus */
      while (0 != osSemaphoreGetCount(Sem_tcp_reciv_sem)) 
        osSemaphoreAcquire(Sem_tcp_reciv_sem, 0);        
      MX_SPI1_Init();                 
    }
    break;
    //===========================================================================  
  case SOCK_INIT:
    if(listen(sock_number)!= SOCK_OK)
    {
      disconnect(sock_number);
      close(sock_number);
    }
    break;
    //==============================================================================    
  case SOCK_LISTEN:
    return sock_sn;
    break;
    //==============================================================================
  default:
    break;
    
  } // end of switch
  
  return sock_sn;
}


int8_t w5500_full_reset(void)
{
  MX_SPI1_Init();
  w5500_init();
  wizphy_reset();
  wizphy_setphyconf(&phy_param);
  wizphy_getphystat(&phy_param);
  wizchip_getnetinfo(&net_param_read);
  return wizphy_getphylink();   
}


 


