#ifndef _AT45DB641E_H_
#define _AT45DB641E_H_


/* Модуль работы с микросхемой памяти AT45DB641E (64-Mbit/8Mbyte)*/

/* Весь функционал сделан для 256 bytes страниц.
Пожалуйста, перед использованием воспользуйтесь
-> at45_ps_read()  и  at45_set_ps(code);
*/


#include <inttypes.h>

/*!!!using the binary page size (256 bytes)!!!*/


//закоментировать чтобы убрать отладочный вывод
//#define DEBUG_AT45        

//Если включен данный макрос, то используется osDelay
//Если нет, то HAL_Delay
#define RTOS

/* Установить используемые входы и выходы */
/*  SPI1  STM32F405RG   
PC3     ------> CS
PA5     ------> SPI1_SCK
PA6     ------> SPI1_MISO
PA7     ------> SPI1_MOSI   */

#define CS_PIN          GPIO_PIN_3
#define CS_PORT         GPIOC

#define SPI1_SCK        GPIO_PIN_5
#define SPI1_MISO       GPIO_PIN_6
#define SPI1_MOSI       GPIO_PIN_7
#define SPI1_PORT       GPIOA

#define CS_SET          HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
#define CS_RESET        HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

//Тайминги
#define TIME_OUT_SPI    (60U)
#define MAX_PAGE_TIME   (5U)    //doc 3ms
#define MAX_BYTE_TIME   (1U)    //doc 1us ! HAL == 1msec
#define MAX_PAGE_ER_T   (40U)   //doc 35ms


//Commands(opcods)
#define READ_ID         ((uint8_t)(0x9F))       //read identification information
#define READ_SR         ((uint8_t)(0xD7))       //read status register
#define CAR_HFM         ((uint8_t)(0x0B))       //Continuous Array Read (High Frequency Mode: 0Bh Opcode)

#define SIZE_256        ((uint8_t)(0xA6))       //Page size 256 bytes command
#define SIZE_264        ((uint8_t)(0xA7))       //Page size 256 bytes command 

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


//Карта памяти работы с файлами
#define ALL_PAGES      (32768UL)               //страниц памяти от 0 - 32767
#define ALL_SIZE       (ALL_PAGES*256UL)       //общий размер в байтах 
#define WRITE_ADT_ST   (0x00)
#define PAGE_SIZE      (256U)
#define ONE_MB         (1024U)
#define PAGE_X_2       (512U)


//смотри datasheet (Chip Erase Time) + 2 sec
#define MAX_FULL_ERASE_TIME  (210U * 1000U) //максимум стирания 208 сек(тут в мсек)

#define AT45_ERR        (0)
#define AT45_OK         (1)
//#define AT45_BUSY       (2)

#define MAX_FILES       (100)                   //Количество треков и инфы о них






  


/**/
void SPI1_Init(void);

/* Чтение идентификационной информации */
uint8_t ad45_test(void);

/* Прочитать регистр статуса */
uint16_t at45_read_status(void);

/* Разбор ргистра статуса */
void parsing_at45_status(uint16_t tb_sr);

/* Сброс at45, используется для приостановки операций */
uint8_t at45_reset(void);

/* Функция проверки готовности к работе */
uint8_t at45_ready(void);

/* Устанновка размера страницы 256/264 */
uint8_t at45_set_ps(uint8_t opc);

/* Проверка размера страницы at45 */
uint8_t at45_ps_read(void);

/* Стирание страницы */
uint8_t at45_page_erase(uint16_t page);

/* Полное стирание данных с at45 */
uint8_t at45_full_erase(uint32_t * free_mem);

/* Загрузить данны в буфер 1 или 2 */
uint8_t at45_load_to_buff(const uint8_t * data,
                          const uint32_t len,
                          const uint8_t addr,
                          const uint8_t bufn);

/* Записать данные из буфера 1 или 2 в основную память */
uint8_t at45_write_to_main(const uint16_t page,
                           const uint8_t bufn,
                           const uint8_t er);

/* Записать сразу 2 страницы */
uint8_t at45_2page_write(uint16_t page, uint8_t * src, uint32_t len);

/* Запись данных в страницу at45 целого буфера(1 или 2) без стирания */
uint8_t at45_page_write_noer(const uint16_t page, 
                             const uint8_t addr,
                             const uint8_t * src, 
                             const uint32_t len,
                             const uint8_t bufn);

/* Запись данных в страницу at45 целого буфера(1 или 2) с предварительным 
стиранием страницы*/
uint8_t at45_page_write_er(const uint16_t page, 
                           const uint8_t addr,
                           const uint8_t * src, 
                           const uint32_t len,
                           const uint8_t bufn);

/* Чтение данных из страницы at45 по адресу страницы и байта в ней */
uint8_t at45_read_page(const uint16_t page, 
                       const uint8_t addr,
                       uint8_t * dst,
                       const uint32_t len);

/* Чтение данных из страницы at45 по адресу байта от 0 до MAX_BYTE_ADDR */
uint8_t at45_read_byte_adr(uint8_t * dst,
                       const uint32_t addr,
                       const uint32_t len);

/* Запись данных из страницы at45 по адресу байта от 0 до MAX_BYTE_ADDR */
uint8_t at45_write_byte_adr(uint8_t * dst,
                       const uint32_t addr,
                       const uint32_t len);

/**/

#endif
