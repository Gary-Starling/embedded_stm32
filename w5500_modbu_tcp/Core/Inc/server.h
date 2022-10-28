#ifndef __SERVER_H__
#define __SERVER_H__


#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA
#define B1_EXTI_IRQn EXTI0_IRQn
#define CS_SPI_Pin GPIO_PIN_4
#define CS_SPI_GPIO_Port GPIOC
#define W5500_INT_Pin GPIO_PIN_5
#define W5500_INT_GPIO_Port GPIOC
#define W5500_INT_EXTI_IRQn EXTI9_5_IRQn
#define W5500_RESET_Pin GPIO_PIN_0
#define W5500_RESET_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define LD4_Pin GPIO_PIN_12
#define LD4_GPIO_Port GPIOD
#define LD3_Pin GPIO_PIN_13
#define LD3_GPIO_Port GPIOD
#define LD5_Pin GPIO_PIN_14
#define LD5_GPIO_Port GPIOD
#define LD6_Pin GPIO_PIN_15
#define LD6_GPIO_Port GPIOD


#define DEBUG                      1
#define MAX_SOCK                   8            //

#define IP_NOT_SET                 (net_param.ip[0] != net_param_read.ip[0] ||\
net_param.ip[1] != net_param_read.ip[1] ||\
  net_param.ip[2] != net_param_read.ip[2] ||\
    net_param.ip[3] != net_param_read.ip[3])

#define MASK_NOT_SET            (net_param.sn[0] != net_param_read.sn[0] ||\
net_param.sn[1] != net_param_read.sn[1] ||\
  net_param.sn[2] != net_param_read.sn[2] ||\
    net_param.sn[3] != net_param_read.sn[3])

#define GW_NOT_SET              (net_param.gw[0] != net_param_read.gw[0] ||\
net_param.gw[1] != net_param_read.gw[1] ||\
  net_param.gw[2] != net_param_read.gw[2] ||\
    net_param.gw[3] != net_param_read.gw[3])

/************************
* tcp process states list   
*************************/    

/*
enum Tcp_state {
  TCP_IDLE,
  TCP_REQ_INPROC,
  TCP_REQ_DONE,
  TCP_RES_INPROC,
  TCP_RES_DONE,
};
*/

#define TCP_IDLE                0           /* IDLE, Waiting for data received (TCP established) */
#define TCP_REQ_INPROC  		1           /* Received request from client */
#define TCP_REQ_DONE    		2           /* The end of request parse */
#define TCP_RES_INPROC  		3           /* Sending the response to client (in progress) */
#define TCP_RES_DONE    		4           /* The end of response send */

#define RECV_IR_MSK                   0x04
#define configMAX_SIZE                2048
#define configPORT                    502




/* w5500 read/write funtions */           
void w5500_sel(void);
void w5500_unsel(void);
void w5500_read_buff(uint8_t* buff, uint16_t len);
void w5500_write_buff(uint8_t* buff, uint16_t len);
uint8_t w5500_read_byte(void);
void w5500_write_byte(uint8_t byte);
void w5500_init(void);
void w5500_enter_critical(void);
void w5500_exit_critical(void);
uint8_t tcp_ip_ser_run(uint8_t sock_number);
int8_t w5500_full_reset(void);
/* w5500 read/write funtions */  


#endif