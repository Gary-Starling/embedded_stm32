
#include "at45db641e.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "cmsis_os2.h"


SPI_HandleTypeDef hspi1;

extern void Error_Handler(void);

void SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}



/**
Функция считывает идентификационная информация
без параметров
return AT45_OK - если чтение прошло успешно
AT45_ERR - если чтение с ошибкой
*/
uint8_t ad45test(void) 
{
  
  HAL_StatusTypeDef result_1, result_2;
  uint8_t opcode = READ_ID;
  uint8_t devid_res[5];
  
  CS_RESET;
  result_1 = HAL_SPI_Transmit(&hspi1, &opcode, sizeof(opcode), TIME_OUT_SPI);
  result_2 = HAL_SPI_Receive(&hspi1, devid_res, sizeof(devid_res), TIME_OUT_SPI);
  CS_SET;
  
  if((result_1 != HAL_OK) || (result_2 != HAL_OK)) 
  {
#ifdef DEBUG_AT45
    printf("Error during getting the device id, res1 = %d, res2 = %d\r\n", result_1, result_2);
#endif
    return AT45_ERR;
  }
  else
  {  
#ifdef DEBUG_AT45
    printf("Manufacturer ID: 0x%02X\r\n"
           "Device ID (byte 1): 0x%02X\r\n"
             "Device ID (byte 2): 0x%02X\r\n"
               "Extended device information (EDI) string length: 0x%02X\r\n"
                 "EDI byte 1: 0x%02X\r\n"
                   "--------\r\n",
                   devid_res[0], devid_res[1], devid_res[2],
                   devid_res[3], devid_res[4]);
#endif
    return AT45_OK;
  }
}


/** Чтение регистра статуса(Этот регистр только для чтения)
Без параметров
retrun - возращает два байта регистра статуса 
В случае ошибки возращает 0xFFFF
*/
uint16_t at45_read_status(void)
{
  HAL_StatusTypeDef result_1, result_2;
  uint16_t result;
  uint8_t opcode = READ_SR;
  
  CS_RESET;
  result_1 = HAL_SPI_Transmit(&hspi1, &opcode , sizeof(opcode), MAX_BYTE_TIME);
  result_2 = HAL_SPI_Receive(&hspi1, (uint8_t *)(&result), sizeof(result), MAX_BYTE_TIME * sizeof(opcode));
  CS_SET;
  
  if((result_1 != HAL_OK) || (result_2 != HAL_OK))
  {
#ifdef DEBUG_AT45
    printf("Error read status register\n");
#endif
    return 0xFFFF;
  }
  else
  {
    parsing_at45_status(result);
    return result;
  }
  
}


/**
Парсинг регистра статуса, сделан для отладки
tb_sr - два байт регистра статуса
noreturn
*/
void parsing_at45_status(uint16_t tb_sr)
{
  uint8_t status[2];
  
  status[0] = (uint8_t)(tb_sr);         //byte - 1
  status[1] = (uint8_t)(tb_sr >> 8);    //byte - 2
  
#ifdef DEBUG_AT45
  
  if(status[0] & (1 << 7)) 
    printf("Device is ready\n");
  else
    printf("Device is busy with an internal operation\n");
  
  if(status[0] & (1 << 6)) 
    printf("Main memory page data does not match buffer data.\n");
  else
    printf("Main memory page data matches buffer data\n");
  
  if(status[0] & (1 << 1)) 
    printf("Sector protection is enabled.\n");
  else
    printf("Sector protection is disabled.\n");
  
  if(status[0] & (1 << 0)) 
    printf("Device is configured for \"power of 2\" binary page size (256 bytes).\n");
  else
    printf("Device is configured for standard DataFlash page size (264 bytes).\n");
  
  if(status[1] & (1 << 7))
    printf("Device is ready.\n");
  else
    printf("Device is busy with an internal operation.\n");
  
  if(status[1] & (1 << 5))
    printf("Erase or program error detected.\n");
  else
    printf("Erase or program operation was successful.\n");
  
  if(status[1] & (1 << 3))
    printf("Sector Lockdown command is enabled.\n");
  else
    printf("Sector Lockdown command is disabled.\n");
  
  if(status[1] & (1 << 2))
    printf("A sector is program suspended while using Buffer 2.\n");
  else
    printf("No program operation has been suspended while using Buffer 2.\n");
  
  if(status[1] & (1 << 1))
    printf("A sector is program suspended while using Buffer 1.\n");
  else
    printf("No program operation has been suspended while using Buffer 1.\n");
  
  if(status[1] & (1 << 0))
    printf("A sector is erase suspended.\n");
  else
    printf("No sectors are erase suspended\n");
  
#endif
}


/** Чтение данных из страницы
uint16_t page – номер читаемой страницы (0 – 32767) у нас 8мб памяти
uint8_t addr – адрес байта внутри страницы (0 – 255) 256 бит страница
uint8_t *dst – указатель, куда класть прочитанные данные
uint32_t length – количество читаемых байт
*/
uint8_t at45_read_page(const uint16_t page,
                       const uint8_t addr,
                       uint8_t * dst,
                       const uint32_t len)
{
  //uint8_t  state;     // 0x0b|adr_page|adr_in_page|Dummy
  uint8_t  opcode[5] = { CAR_HFM, 0x00,0x00,0x00,DUMMY8B };        
  
  if( page > ALL_PAGES - 1) 
    return AT45_ERR;
  
  if(!at45_ready())
    return AT45_ERR;
  
  //Формируем адрес читаемой страницы
  opcode[1] = (uint8_t)((page & 0xFF00) >> 8);
  opcode[2] = (uint8_t)((page & 0x00FF) >> 0);
  //адрес внутри страницы
  opcode[3] = (uint8_t)(addr);
  
  /*The CS pin must remain low during the loading of the opcode, 
  the address bytes, the dummy byte, and the reading of data*/
  CS_RESET;
  if(HAL_OK != HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), sizeof(opcode) * MAX_BYTE_TIME))
    return AT45_ERR;
  
  if(HAL_OK != HAL_SPI_Receive(&hspi1, dst, len, MAX_BYTE_TIME * len))
    return AT45_ERR;
  
  CS_SET;
  
  return AT45_OK;
}


///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//TODO: чтение произвольного участка 
uint8_t at45_read_byte_adr(uint8_t * dst,
                  const uint32_t addr,
                  const uint32_t len)
{
  
  if(addr > ALL_SIZE - 1 )
    return AT45_ERR;
  
  if(!at45_read_page(addr / PAGE_SIZE, addr % PAGE_SIZE, dst, len))
    return AT45_ERR;
  
    return AT45_OK;
}

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//TODO: запись в произвольный участок
uint8_t at45_write_byte_adr(uint8_t * dst,
                            const uint32_t addr,
                            const uint32_t len)
{
  return AT45_OK;
}

/**
Запись данных в один из доступных буферов
const uint8_t * data - указатель на данные
const uint32_t len - длина передаваемых данных             
const uint8_t bufn - номер используемого буфера
const uint8_t addr - адрес(0-255) для записи в буфер
return AT45_OK/AT45_ERR; 
*/
uint8_t at45_load_to_buff(const uint8_t * data,
                          const uint32_t len,
                          const uint8_t addr,
                          const uint8_t bufn)
{
  //--------------------comm | 16 dummy bits | 8 bit adress
  uint8_t opcode[4]  = {0x00, DUMMY8B, DUMMY8B, addr}; 
  
  if(len > 256U) return AT45_ERR;              //max len buff 256 byte
  
  switch(bufn)
  {
  case BUFFER1: opcode[0] = LOAD_BUF1; break;   //load to buf1
  case BUFFER2: opcode[0] = LOAD_BUF2; break;   //load to buf2
  default: return AT45_ERR; break;              //wrong operation
  }
  
  CS_RESET;                                     
  if(HAL_OK != HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), MAX_BYTE_TIME * sizeof(opcode)))
    return AT45_ERR;
  if(HAL_OK != HAL_SPI_Transmit(&hspi1, (uint8_t *)data, len, MAX_BYTE_TIME * len))
    return AT45_ERR;
  CS_SET;
  
  return AT45_OK;
}


/**
Запись буфера 1 или 2, в страницу основной памяти со стиранием или без
const uint16_t page - номер страницы(0-32767)
const uint8_t bufn - BUFFER1/BUFFER2(из какого буфера писать в память)
const uint8_t er
return AT45_OK/AT45_ERR; 
*/
uint8_t at45_write_to_main(const uint16_t page,
                           const uint8_t bufn,
                           const uint8_t er)
{
  //---------------------------com|15b addr+1 dummy bit|8dummy bits 
  uint8_t com_page_addr[4] = {0x00,0x00,0x00,DUMMY8B};
  
  switch(bufn)
  {
    
  case BUFFER1: //write to memory data from buf1 
    if(er)
      com_page_addr[0] = WR_FR_BUF1_E;
    else
      com_page_addr[0] = WR_FR_BUF1_NOE;
    break;   
    
  case BUFFER2: //write to memory data from buf2
    if(er)
      com_page_addr[0] = WR_FR_BUF2_E;
    else
      com_page_addr[0] = WR_FR_BUF2_NOE;
    break;   
    
  default: return AT45_ERR; break;         //wrong operation
  
  }
  
  /* формируем адрес страницы */
  com_page_addr[1] = (uint8_t)( (page & 0xFF00) >> 8);          //MSB
  com_page_addr[2] = (uint8_t)( (page & 0x00FF) >> 0);          //LSB
  
  /* Запишем данные из буфера N, по адресу page_addr */
  CS_RESET; 
  if(HAL_OK != HAL_SPI_Transmit(&hspi1, com_page_addr, sizeof(com_page_addr), MAX_BYTE_TIME * sizeof(com_page_addr)))
    return AT45_ERR;
  CS_SET;

  return AT45_OK;
}



/* Запись в страницу at45 без стирания
Страница должна быть стёрта перед записью.
Записывается целый буфер.

const uint16_t page - номер страницы от 0 до 32767
const uint8_t addr - с какого байта писать в буфер(0-255)
const uint8_t * src - указатель на записываемые данные
const uint32_t length - длина передаваемых данных в байтах
const uint8_t bufn - нз какого буфера идёт запись в память
return - AT45_ERR/AT45_OK;
*/
uint8_t at45_page_write_noer(const uint16_t page, 
                        const uint8_t addr,
                        const uint8_t * src, 
                        const uint32_t len,
                        const uint8_t bufn)
{ 
  /* Данные больше чем страница памяти или максимумальное количество страниц */
  if( len > PAGE_SIZE || page > ALL_PAGES - 1) 
    return AT45_ERR;
  
  if(!at45_ready())
    return AT45_ERR;
  
  /* отпправляем ком.записи в буфер и наполняем данными */ 
  if(!at45_load_to_buff(src, len, addr, bufn))
    return AT45_ERR;
  
  /* Запишем данные из буфера по адресу page */
  if(!at45_write_to_main(page, bufn, 0))
    return AT45_ERR;
  
  if(!at45_ready())
    return AT45_ERR;
  
  return AT45_OK;
}

/* Запись в страницу at45 с предварительным стиранием.
Записывается целый буфер
const uint16_t page - номер страницы от 0 до 32767
const uint8_t addr - с какого байта писать в буфер(0-255)
const uint8_t * src - указатель на записываемые данные
const uint32_t length - длина передаваемых данных в байтах
const uint8_t bufn - нз какого буфера
return - AT45_ERR/AT45_OK;
*/
uint8_t at45_page_write_er(const uint16_t page, 
                        const uint8_t addr,
                        const uint8_t * src, 
                        const uint32_t len,
                        const uint8_t bufn)
{ 
  /* Данные больше чем страница памяти или максимумальное количество страниц */
  if( len > PAGE_SIZE || page > ALL_PAGES - 1) 
    return AT45_ERR;
  
  if(!at45_ready())
    return AT45_ERR;
  
  /* отпправляем ком.записи в буфер и наполняем данными */ 
  if(!at45_load_to_buff(src, len, addr, bufn))
    return AT45_ERR;
  
  /* Запишем данные из буфера по адресу page */
  if(!at45_write_to_main(page, bufn, 1))
    return AT45_ERR;
  
  if(!at45_ready())
    return AT45_ERR;
  
  return AT45_OK;
}

/** 
Функция записи двух страниц памяти
NOTE: Важно!!! Пишем по 2 страницы !!!
Для ускорения записи и мы будем писать по 512 байт, 256 в буф1 и 256 в буф2;
т.е. открываем файл, пусть 1.wav на 61,3 КБ (62 820 байт)
62280 / 256(одна страница) = 245 записей, остаток 100 байт;
но т.к. мы пишем по странице, вместо 100 байт мы запишем 256, +256 байт.
Итого - 247 записей.
Мы проигрываем в использовании памяти, но упрощаем адресацию.

uint16_t page, uint8_t * src, uint32_t len
*/
uint8_t at45_2page_write(uint16_t page, uint8_t * src, uint32_t len)
{
  if(len > (PAGE_X_2) || page > ALL_PAGES - 1 ) 
    return AT45_ERR;
  
  if(!at45_page_write_noer(page, 0x00, src, len/2, BUFFER1))
    return AT45_ERR;
  
  if(!at45_page_write_noer(page + 1, 0x00, src + 256, len/2, BUFFER2))
    return AT45_ERR;
  
  return AT45_OK;
}
     
     

/** Функция установки размера страницы 256 байт
uint8_t opcode - SIZE_256 или SIZE_264
return AT45_OK или return AT45_ORR;
*/
uint8_t at45_set_ps(uint8_t opc)
{
  uint8_t  res;
  uint32_t opcode = 0x00802A3D | (opc << 24);
  
  /* Проверка на ввод правильной команды */
  if( (opc != SIZE_256) && (opc != SIZE_264) )       
    return AT45_ERR;
  
  if(!at45_ready())
    return AT45_ERR;
  
  CS_RESET;
  res = HAL_SPI_Transmit(&hspi1, (uint8_t *)&opcode, sizeof(opcode), HAL_MAX_DELAY);
  CS_SET;
  
  if(res != HAL_OK)
    return AT45_ERR;
  
  return AT45_OK;
}

/**
Функция проверки установленного размера страницы
без аргументов
return value SIZE_256/SIZE_264/AT45_ERR - в случае проблемы работы с spi
*/
uint8_t at45_ps_read(void)
{
  HAL_StatusTypeDef res1, res2;
  uint8_t result[2];            //но нам нужен только первый байт
  uint8_t opcode = READ_SR;
  
  CS_RESET;
  res1 = HAL_SPI_Transmit(&hspi1, &opcode , sizeof(opcode), TIME_OUT_SPI);
  res2 = HAL_SPI_Receive(&hspi1, (uint8_t *)(&result), sizeof(result), TIME_OUT_SPI);
  CS_SET;
  
  if((res1 != HAL_OK) || (res2 != HAL_OK))
    return AT45_ERR;
  else
  {
    if(result[0] & (1 << 0))
      return SIZE_256;
    else
      return SIZE_264;
  }
}



/** Полное стирание at45 Важно!!! от 80 до 208s по докам.
uin32_t * free_mem - указатель на свободное место в at45
return - AT45_ERR/AT45_OK
*/
uint8_t at45_full_erase(uint32_t * free_mem)
{
  uint32_t timeout = 0;
  uint8_t res;
  uint32_t opcode = FULL_ERASE;
  
  
  if(!at45_ready())
    return AT45_ERR;
  
  CS_RESET;
  res = HAL_SPI_Transmit(&hspi1, (uint8_t *)&opcode, sizeof(opcode), HAL_MAX_DELAY);
  CS_SET;
  
  if(res != HAL_OK)
    return AT45_ERR;
  
  while (!(at45_read_status() & 0x80))  
  {
    
#ifdef RTOS
    osDelay(1);
#else
    HAL_Delay(1);
#endif
    timeout++;
    
    if(timeout >= MAX_FULL_ERASE_TIME)
      return AT45_ERR;
  } 
  
  *free_mem = ALL_SIZE;
  
#ifdef DEBUG_AT45           
  printf("at45 free memory = %d Mbyte\r\n", ALL_SIZE/ONE_MB);
#endif
  
  return AT45_OK;
}

/** Функция стирания 
*/
uint8_t at45_page_erase(uint16_t page)
{
  uint8_t res;
  uint8_t cmd[4] = {
    PGERASE,
    (uint8_t)(page >> 8), //msb
    (uint8_t)(page)    ,  //lsb
    DUMMY8B
  };
  
  if(!at45_ready())
    return AT45_ERR;
  
  
  if(page > ALL_PAGES - 1)
    return AT45_ERR;
  
  CS_RESET;
  res = HAL_SPI_Transmit(&hspi1, cmd, sizeof(cmd), 256 * MAX_BYTE_TIME);
  CS_SET;
  
  if(!at45_ready())
    return AT45_ERR;
  
  //
  return AT45_OK;
}



/* Функция рестарта at45
Используется для немедленной приостановки операций
Без аргументов
return AT45_OK - в случае успешного рестарта
ERR - ошибка*/
uint8_t at45_reset(void)
{
  uint32_t opcode = RESTART;
  
  //Готова ли микросхема к ресесту?
  if(!at45_ready())
    return AT45_ERR;
  
  
  CS_RESET;
  HAL_SPI_Transmit(&hspi1, (uint8_t *)(&opcode), sizeof(opcode), TIME_OUT_SPI);
  CS_SET;
  
#ifdef DEBUG_AT45
  printf("Restart at45... \r\n");
#endif
  
  if(!at45_ready())
    return AT45_ERR;
  
  return AT45_OK;  
}

/**  Функция проверки готовности at45 к транзакциям
note: При записи или стирания, at45 будет устанавливать в регистре статуса
флаг BUSY;

без аргументов
return AT45_OK - если at45 готова к транзакциям
AT45_ERR - в случае BUSY
*/
uint8_t at45_ready(void)
{
  uint16_t timeout = 0;
  
  while (!(at45_read_status() & 0x0080)) 
  {
    timeout++;
    
    if(timeout >= 1000U)
      return AT45_ERR; 
  } 
  
  return AT45_OK;
}


