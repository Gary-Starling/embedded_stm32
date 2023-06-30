#ifndef _SIM7600_H_
#define _SIM7600_H_

/*
          default value
115200bps, 8 bit data, no parity, 1 bit stop, no data stream control.
*/

#include <stdint.h>

//settings
#define SIM_RTOS         1
//TODO: complete not rtos
#define SIM_DEBUG        1
#define MAX_SEMAPHORE   (100UL)
#define MAX_EXTRA_MES   (10UL)
//UART
#define DEF_BAUD_RATE   (115200UL)


/* Структура ответов от парсера */
#define ANSWER_SIZE     (641UL)
typedef struct {
  char buff[ANSWER_SIZE];
} sAnswrObject;

/* Варианты ответов функции sim76xx_send_at_com */
typedef enum {
  AT_SEND_OK = 0,
  AT_UART_DMA_ERROR,
  AT_MQ_ERROR,
  AT_WR_ANSWER,
  AT_TIME_OUT,
  AT_ERR_PARAM,
  AT_ERR_REPSP
} eComRes;


/* Возможные экстра сообщения */
typedef enum {
  RING,
  END_CALL,
  MISSED_CALL,
  RXDTMF,
  SMS,
  SMS_FULL,
  NO_CARRIER,
  SIM_NOT_AVL,
  SIM_NOT_INS,
  ERR_INIT
}eMsgExtra;

/* структура с экстра сообщения, в ней будет параметр */
typedef struct {
 eMsgExtra msg;
 char val;
}sExtraMsg;


/**USART3 GPIO Configuration
PB10     ------> UART3_TX
PB11     ------> UART3_RX
*/
#define UART_TX_PIN  GPIO_PIN_10
#define UART_RX_PIN  GPIO_PIN_11
#define UART_PORT    GPIOB

//PWRKEY
#define PWR_KEY_PORT   GPIOB
#define PWR_KEY_PIN    GPIO_PIN_1
#define PWR_KEY_ON     HAL_GPIO_WritePin(PWR_KEY_PORT, PWR_KEY_PIN, GPIO_PIN_SET)
#define PWR_KEY_OFF    HAL_GPIO_WritePin(PWR_KEY_PORT, PWR_KEY_PIN, GPIO_PIN_RESET)

//custom pwr
#define GSM_PORT       GPIOC
#define GSM_PIN        GPIO_PIN_8
#define GSM_ON         HAL_GPIO_WritePin(GSM_PORT, GSM_PIN, GPIO_PIN_SET)
#define GSM_OFF        HAL_GPIO_WritePin(GSM_PORT, GSM_PIN, GPIO_PIN_RESET)

//reset pin
#define RESET_PORT     GPIOB
#define RESET_PIN      GPIO_PIN_1
#define RESET_ON       HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_SET)
#define RESET_OFF      HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_RESET)


//custom comm
//use this command to wait for some response
//example :sim76xx_send_at_com(WAIT_STATE, "RDY", NULL, 16000))
#define WAIT_STATE     ((const char *)("WAIT STATE"))

//baudrate uart
#define BAUDR_115200   ((const char *)("AT+IPR=115200\r\n"))
#define BAUDR_19200   ((const char *)("AT+IPR=19200\r\n"))
//uart echo
#define ECHO_OFF       ((const char *)("ATE0\r\n"))
#define ECHO_ON        ((const char *)("ATE1\r\n"))

//custom up board and led's commands
#define PIN1_GPIO       ((const char *)("AT+CGFUNC=3,0\r\n"))
#define PIN2_GPIO       ((const char *)("AT+CGFUNC=6,0\r\n"))
#define PIN3_GPIO       ((const char *)("AT+CGFUNC=43,0\r\n"))
#define PIN4_GPIO       ((const char *)("AT+CGFUNC=41,0\r\n"))
#define PIN5_GPIO       ((const char *)("AT+CGFUNC=77,0\r\n"))

//leds
#define VD1_OUT         ((const char *)("AT+CGDRT=3,1\r\n"))
#define VD1_INPUT       ((const char *)("AT+CGDRT=3,0\r\n"))

#define VD2_OUT         ((const char *)("AT+CGDRT=6,1\r\n"))
#define VD2_INPUT       ((const char *)("AT+CGDRT=6,0\r\n"))

#define VD3_OUT         ((const char *)("AT+CGDRT=43,1\r\n"))
#define VD3_INPUT       ((const char *)("AT+CGDRT=43,0\r\n"))

#define VD4_OUT         ((const char *)("AT+CGDRT=41,1\r\n"))
#define VD4_INPUT       ((const char *)("AT+CGDRT=41,0\r\n"))

#define VD5_OUT         ((const char *)("AT+CGDRT=77,1\r\n"))
#define VD5_INPUT       ((const char *)("AT+CGDRT=77,0\r\n"))

#define VD1_SET         ((const char *)("AT+CGSETV=3,1\r\n"))
#define VD1_RESET       ((const char *)("AT+CGSETV=3,0\r\n"))

#define VD2_SET         ((const char *)("AT+CGSETV=6,1\r\n"))
#define VD2_RESET       ((const char *)("AT+CGSETV=6,0\r\n"))

#define VD3_SET         ((const char *)("AT+CGSETV=43,1\r\n"))
#define VD3_RESET       ((const char *)("AT+CGSETV=43,0\r\n"))

#define VD4_SET         ((const char *)("AT+CGSETV=41,1\r\n"))
#define VD4_RESET       ((const char *)("AT+CGSETV=41,0\r\n"))

#define VD5_SET         ((const char *)("AT+CGSETV=77,1\r\n"))
#define VD5_RESET       ((const char *)("AT+CGSETV=77,0\r\n"))

//restart command
#define RES_MODULE      ((const char *)("AT+CRESET\r\n"))

//Set functionality
#define REQUEST_FUNC    ((const char *)("AT+CFUN=?\r\n"))
#define SET_FULL_FUNC   ((const char *)("AT+CFUN=1\r\n"))
#define SET_MINI_FUNC   ((const char *)("AT+CFUN=0\r\n"))

// call
#define ANSWR_CALL      ((const char *)("ATA\r\n"))
#define STOP_CALL       ((const char *)("AT+CHUP\r\n"))
#define PLAY_DATA       ((const char *)("AT+CCMXPLAY=?\r\n"))

//dtmf
#define DTMF_TONE_LIST  ((const char *)("AT+VTD=?\r\n"))
#define DTMF_TONE_INST  ((const char *)("AT+VTD?\r\n"))
#define SIDET_EN        ((const char *)("AT+SIDET=1\r\n"))
#define SIDET_DI        ((const char *)("AT+SIDET=0\r\n"))

//pcm
#define PCM_RATE        ((const char *)("AT+CPCMBANDWIDTH?\r\n"))
#define PCM_RATE_8KHZ   ((const char *)("AT+CPCMBANDWIDTH=1,1\r\n"))

//TODO: macros DEL_SMS(M)
//sms
/*Delete all messages from preferred message storage
including unread messages.*/
#define DEL_ALL_SMS     ((const char *)("AT+CMGD=,4\r\n"))

/*Delete all read messages from preferred message
storage, sent and unsent mobile originated messages
leaving unread messages untouched.*/
#define DEL_ALLRS_SMS   ((const char *)("AT+CMGD=,3\r\n"))

/*Delete all read messages from preferred message*/
#define DEL_ALLR_SMS    ((const char *)("AT+CMGD=2\r\n"))
#define SMS_FORMAT_LIST ((const char *)("AT+CMGF=?\r\n"))
#define SMS_FORMAT_INST ((const char *)("AT+CMGF?\r\n"))
#define SMS_FORMAT_TXT  ((const char *)("AT+CMGF=1\r\n"))
#define SMS_FORMAT_PDU  ((const char *)("AT+CMGF=0\r\n"))

//internet
/* This command is used to Read the ICCID from SIM card */
#define READ_ICCID      ((const char *)("AT+CICCID\r\n"))
/* network registration */
#define LIST_CREG       ((const char *)("AT+CREG?\r\n"))
/* mode ? Automatic/GSM Only/CDMA Only... WCDMA+LTE Only */
#define LIST_MODE       ((const char *)("AT+CNMP?\r\n"))
/* gprs enable */
#define GPRS_ENABLE     ((const char *)("AT+CGATT=1\r\n"))
/* enable pdp conetxt */
#define PDP_ENABLE      ((const char *)("AT+CGACT=1,1\r\n"))
/* apn for MEGAFON !!! see your provider */
#define APN_MEGAFON     ((const char *)("AT+CGDCONT=1,\"IP\",\"internet\"\r\n"))
/* ip ?*/
#define IP_ADDR         ((const char *)("AT+CGPADDR\r\n"))

void init_uart(void);
void deinit_uart(void);

/* Включение sim7600 и uart */
eComRes sim76xx_init(void);

/* Функция ожидания включения модуля */
eComRes sim76xx_pwr_up_wait(void);

/* Функция установки пользовательских настроек */
eComRes sim76xx_set_settings(void);

/* Инит пинов uart */
eComRes sim76xx_gpio_init(void);

/* Инит пинов uart */
eComRes sim76xx_internet_init(void);

/* Перезапуск uart и соединения с модулем */
void sim76xx_uart_reset(void);

/* Функция мигания светодиодами от кастомной платы*/
void sim76xx_led_blink(uint32_t time, uint8_t mode);

/* Рестарт модуля hard */
void hard_sim76xx_reset(void);

/* Рестарт модуля soft */
void soft_sim76xx_reset(void);

/* Отправка AT-com модулю */
eComRes sim76xx_send_at_com(const char * com,
                            const char * wait_ans,
                            char * const copy,
                            uint32_t max_t);

/* Парсинг кольцевого буфера */
void sim76xx_parsing(void * rec_buff);

/* Ответить на звонок */
eComRes sim76xx_answ_call(void);
#endif