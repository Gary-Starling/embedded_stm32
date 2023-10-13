#ifndef _AT45DB641E_H_
#define _AT45DB641E_H_

/* Memory chip module AT45DB641E (64-Mbit/8Mbyte)*/

/*
All functionality is made for 256 bytes pages.
Please use
-> at45_ps_read()  и  at45_set_ps(code);
before start
*/


#include <inttypes.h>
#include <stddef.h>

/*!!!using the binary page size (256 bytes)!!!*/
//#define DEBUG_AT45

//If this macro is enabled, then osDelay is used; if not, then delay_ms()
#define RTOS

/*  SPI1  STM32F405RG
PC3     ------> CS
PA5     ------> SPI1_SCK
PA6     ------> SPI1_MISO
PA7     ------> SPI1_MOSI   */

#define GPIO_PIN_3     ((uint16_t)0x0008)
#define GPIO_PIN_5     ((uint16_t)0x0020)
#define GPIO_PIN_6     ((uint16_t)0x0040)
#define GPIO_PIN_7     ((uint16_t)0x0080)

#define CS_PIN          GPIO_PIN_3
#define CS_PORT         GPIOC

#define SPI1_SCK        GPIO_PIN_5
#define SPI1_MISO       GPIO_PIN_6
#define SPI1_MOSI       GPIO_PIN_7
#define SPI1_PORT       GPIOA

#define CS_SET          (CS_PORT->BSRR = GPIO_PIN_3)
#define CS_RESET        (CS_PORT->BSRR = (uint32_t)GPIO_PIN_3 << 16U)

//timing
#define TIME_OUT_SPI    (60U)
#define MAX_PAGE_TIME   (5U)    //doc 3ms
#define MAX_BYTE_TIME   (100U)    //doc 1us ! tim1 == 1msec
#define MAX_PAGE_ER_T   (40U)   //doc 35ms

//Commands(opcods)
#define READ_ID         ((uint8_t)(0x9F))       //read identification information
#define READ_SR         ((uint8_t)(0xD7))       //read status register
#define CAR_HFM         ((uint8_t)(0x0B))       //Continuous Array Read (High Frequency Mode: 0Bh Opcode)

#define SIZE_256        ((uint8_t)(0xA6))       //Page size 256 bytes command
#define SIZE_264        ((uint8_t)(0xA7))       //Page size 264 bytes command

#define RESTART         ((uint32_t)(0x000000f0))//Software Reset

#define BUFFER1         ((uint8_t)(0x01))
#define BUFFER2         ((uint8_t)(0x02))

#define LOAD_BUF1       ((uint8_t)(0x84))       //Load data into Buffer 1
#define LOAD_BUF2       ((uint8_t)(0x87))       //Load data into Buffer 2

#define WR_FR_BUF1_NOE  ((uint8_t)(0x88))       //write from Buffer 1 without Built-In Erase
#define WR_FR_BUF2_NOE  ((uint8_t)(0x89))       //write from Buffer 2 without Built-In Erase

#define WR_FR_BUF1_E    ((uint8_t)(0x83))       //write from Buffer 1 with Built-In Erase
#define WR_FR_BUF2_E    ((uint8_t)(0x86))       //write from Buffer 2 with Built-In Erase

#define DUMMY8B         ((uint8_t)(0x00))       //8 dummy bits

#define PGERASE         ((uint8_t)(0x81))       //Page erase
#define BLOCK_ERASE     ((uint8_t)(0x50))       //Block erase
#define FULL_ERASE      ((uint32_t)(0x9A8094C7))//Chip Erase

//Memory map
#define ALL_PAGES      (32768UL)
#define ALL_SIZE       (ALL_PAGES*256UL)
#define WRITE_ADT_ST   (0x00)
#define PAGE_SIZE      (256U)
#define ONE_MB         (1024U)
#define PAGE_X_2       (512U)

//see datasheet (Chip Erase Time) + 2 sec
#define MAX_FULL_ERASE_TIME  (210U * 1000U)

typedef enum
{
  AT_45_OK = 0,
  AT_45_ER_PAR,
  AT_45_SPI_BUSY,
  AT_45_SPI_TIMOUT,
  AT_45_COMM_ERR
}eAt45status;


/**/
void at45db_init(void);

/* Reading identification information */
eAt45status at45_test(void);

/*
Read status register */
uint16_t at45_read_status(void);

/* Parsing the status register */
void parsing_at45_status(uint16_t tb_sr);

/* Reset at45, used to suspend operations */
eAt45status at45_reset(void);

/* checking ready to work */
eAt45status at45_ready(void);

/* Page size setting 256/264 */
eAt45status at45_set_ps(uint8_t opc);

/* Page size check */
uint8_t at45_ps_read(void);

/* Page Erase */
eAt45status at45_page_erase(uint16_t page);

/* Full erases */
eAt45status at45_full_erase(uint32_t * free_mem);

/* Load data into buffer 1 or 2 */
eAt45status at45_load_to_buff(const uint8_t * data,
                              const uint32_t len,
                              const uint8_t addr,
                              const uint8_t bufn);

/* Write data from buffer 1 or 2 to main memory*/
eAt45status at45_write_to_main(const uint16_t page,
                               const uint8_t bufn,
                               const uint8_t er);

/* Record 2 page at once */
eAt45status at45_2page_write(uint16_t page, uint8_t * src, uint32_t len);

/* Write data to the at45 page of the whole buffer (1 or 2) without erasing */
eAt45status at45_page_write_noer(const uint16_t page,
                                 const uint8_t addr,
                                 const uint8_t * src,
                                 const uint32_t len,
                                 const uint8_t bufn);

/* Writing data to the at45 page of the whole buffer (1 or 2) with a pre page erase */
eAt45status at45_page_write_er(const uint16_t page,
                               const uint8_t addr,
                               const uint8_t * src,
                               const uint32_t len,
                               const uint8_t bufn);

/* Reading data from an at45 page at the address of the page and the byte in it */
eAt45status at45_read_page(const uint16_t page,
                           const uint8_t addr,
                           uint8_t * dst,
                           const uint32_t len);

/* Read data from at45 page at byte address from 0 to MAX_BYTE_ADDR */
eAt45status at45_read_byte_adr(uint8_t * dst,
                               const uint32_t addr,
                               const uint32_t len);

/* Write data from at45 page to byte address from 0 до MAX_BYTE_ADDR */
eAt45status at45_write_byte_adr(uint8_t * dst,
                                const uint32_t addr,
                                const uint32_t len);

/* Low levlel functions */
extern uint32_t GetTick(void);
extern void delay_ms(uint32_t d);
eAt45status spi_transmit(const uint8_t * data, size_t size, uint32_t timeout);
eAt45status spi_recive(uint8_t * rdata, size_t size, uint32_t timeout);
#endif
