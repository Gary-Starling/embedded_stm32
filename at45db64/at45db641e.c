#include <stdio.h>
#include "cmsis_os2.h"
#include "at45db641e.h"
#include "stm32f405xx.h"



static void spi1_init(void);
static void spi1_pin_enable(void);
extern void Error_Handler(void);

/* init at45db external flash */
void at45db_init(void)
{
  uint32_t att = 0;

  spi1_init();

  do {
    att++;
    if(att >= 10) Error_Handler();
  }while(AT_45_OK != at45_test());
}

/* spi pin enable */
static void spi1_pin_enable(void)
{

  /*    SPI1 GPIO
  PC3     ------> CS
  PA5     ------> SPI1_SCK
  PA6     ------> SPI1_MISO
  PA7     ------> SPI1_MOSI
  */
  CS_RESET;

  /* AF pa5,6,7 */
  SPI1_PORT->MODER |= ( GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
  /* HSpeed */
  SPI1_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6  | GPIO_OSPEEDR_OSPEED7);
  /* AFunc */
  SPI1_PORT->AFR[0] |= ( (5 << GPIO_AFRL_AFSEL5_Pos) | (5 << GPIO_AFRL_AFSEL6_Pos)\
    | (5 << GPIO_AFRL_AFSEL7_Pos) );

  /* cs output */
  CS_PORT->MODER |= GPIO_MODER_MODER3_0;
  CS_PORT->OTYPER &= ~GPIO_OTYPER_OT5;
  CS_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;
  CS_PORT->PUPDR |=  GPIO_PUPDR_PUPD5_0;
}

static void spi1_init(void)
{
  spi1_pin_enable();

  SPI1->CR1 &= ~SPI_CR1_SPE;                                /* Disable SPI  */
  SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM);  //master
  SPI1->CR1 &= ~SPI_CR1_DFF;                                 //8bit
  SPI1->CR1 &= ~SPI_CR1_CPOL;                                //CK to 0 when idle
  SPI1->CR1 &= ~SPI_CR1_CPHA;                                //The first clock transition is the first data capture edge
  SPI1->CR1 &= ~SPI_CR1_BR;                                  //fPCLK/2
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST;                            //msb
  SPI1->CR1 &= ~SPI_CR1_CRCEN;                               //crc disable
  SPI1->CR2 &= ~SPI_CR2_FRF;                                 //motrolla
  SPI1->CR2 &= ~SPI_CR2_SSOE;                                //SS output is disabled
  SPI1->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;                      //SPI mode is selected
  SPI1->CR1 |= SPI_CR1_SPE;                                  /* Enable SPI  */
}

/**
*   Function reads identification information
@void
@eAt45status return AT_45_OK
if error
AT_45_ER_PAR,
AT_45_SPI_BUSY,
AT_45_SPI_TIMOUT
*/
eAt45status at45_test(void)
{
  eAt45status res1, res2;
  uint8_t opcode = READ_ID;
  uint8_t devid_res[5];
  uint16_t timeout = 0;

  while (!(at45_read_status() & 0x0080))
  {
#ifdef RTOS
    osDelay(1);
#else
    delay_ms(1);
#endif
    timeout++;

    if(timeout >= 1000U) return AT_45_COMM_ERR;
  }

  CS_RESET;
  res1 = spi_transmit(&opcode, sizeof(opcode), TIME_OUT_SPI);
  res2 = spi_recive(devid_res, sizeof(devid_res), TIME_OUT_SPI);
  CS_SET;

  if(res1 || res2)
  {
#ifdef DEBUG_AT45
    printf("Error during getting the device id, res1 = %d, res2 = %d\r\n", res1, res2);
#endif
    return AT_45_COMM_ERR;
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
    return AT_45_OK;
  }
}


/**
*   Read status register (only read)
@void
@retrun - returns two bytes of the status register
*/
uint16_t at45_read_status(void)
{
  eAt45status res1, res2;
  uint16_t result;
  uint8_t opcode = READ_SR;

  CS_RESET;
  res1 = spi_transmit(&opcode, sizeof(opcode), MAX_BYTE_TIME * sizeof(opcode));
  res2 = spi_recive((uint8_t *)(&result), sizeof(result), MAX_BYTE_TIME * sizeof(result));
  CS_SET;

  if(res1 || res2)
  {
#ifdef DEBUG_AT45
    printf("Error read status register\n");
#endif
    return 0x0000;
  }
  else
  {
    parsing_at45_status(result);
    return result;
  }

}


/**
*  Parsing status reg (for debuging)
@tb_sr - two bytes status reg
@noreturn
*/
void parsing_at45_status(uint16_t tb_sr)
{
#ifdef DEBUG_AT45
  uint8_t status[2];

  status[0] = (uint8_t)(tb_sr);         //byte - 1
  status[1] = (uint8_t)(tb_sr >> 8);    //byte - 2

  if(status[0] & (1 << 7)) printf("Device is ready\n");
  else printf("Device is busy with an internal operation\n");

  if(status[0] & (1 << 6)) printf("Main memory page data does not match buffer data.\n");
  else printf("Main memory page data matches buffer data\n");

  if(status[0] & (1 << 1)) printf("Sector protection is enabled.\n");
  else printf("Sector protection is disabled.\n");

  if(status[0] & (1 << 0)) printf("Device is configured for \"power of 2\" binary page size (256 bytes).\n");
  else printf("Device is configured for standard DataFlash page size (264 bytes).\n");

  if(status[1] & (1 << 7)) printf("Device is ready.\n");
  else printf("Device is busy with an internal operation.\n");

  if(status[1] & (1 << 5)) printf("Erase or program error detected.\n");
  else printf("Erase or program operation was successful.\n");

  if(status[1] & (1 << 3)) printf("Sector Lockdown command is enabled.\n");
  else printf("Sector Lockdown command is disabled.\n");

  if(status[1] & (1 << 2)) printf("A sector is program suspended while using Buffer 2.\n");
  else printf("No program operation has been suspended while using Buffer 2.\n");

  if(status[1] & (1 << 1)) printf("A sector is program suspended while using Buffer 1.\n");
  else printf("No program operation has been suspended while using Buffer 1.\n");

  if(status[1] & (1 << 0)) printf("A sector is erase suspended.\n");
  else printf("No sectors are erase suspended\n");
#endif
}


/**
* Reading data from page
@page – page number to read (0 – 32767) 8mbit
@addr – byte address within the page (0 – 255) 256 bit in page
@dst – where to put data
@length – number of butes read
@return eAt45status (see at45db641e.h)
*/
eAt45status at45_read_page(const uint16_t page,
                           const uint8_t addr,
                           uint8_t * dst,
                           const uint32_t len)
{
  //uint8_t  state;     // 0x0b|adr_page|adr_in_page|Dummy
  uint8_t  opcode[5] = { CAR_HFM, 0x00,0x00,0x00,DUMMY8B };

  if( page > ALL_PAGES - 1) return AT_45_COMM_ERR;

  if(AT_45_OK != at45_ready()) return AT_45_COMM_ERR;


  opcode[1] = (uint8_t)((page & 0xFF00) >> 8);    //prepare page address |
  opcode[2] = (uint8_t)((page & 0x00FF) >> 0);    //                   <-|
  opcode[3] = (uint8_t)(addr);                    //address in page

  /*The CS pin must remain low during the loading of the opcode,
  the address bytes, the dummy byte, and the reading of data*/
  CS_RESET;
  if(AT_45_OK != spi_transmit(opcode, sizeof(opcode), sizeof(opcode) * MAX_BYTE_TIME))
    return AT_45_COMM_ERR;

  if(AT_45_OK != spi_recive(dst, len, MAX_BYTE_TIME * len))
    return AT_45_COMM_ERR;
  CS_SET;

  return AT_45_OK;
}


///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//TODO: reading from random section
eAt45status at45_read_byte_adr(uint8_t * dst,
                               const uint32_t addr,
                               const uint32_t len)
{

  if(addr > ALL_SIZE - 1 )
    return AT_45_COMM_ERR;

  if(AT_45_OK != at45_read_page(addr / PAGE_SIZE, addr % PAGE_SIZE, dst, len))
    return AT_45_COMM_ERR;

  return AT_45_OK;
}

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//TODO: write in randon section
eAt45status at45_write_byte_adr(uint8_t * dst,
                                const uint32_t addr,
                                const uint32_t len)
{
  return AT_45_OK;
}

/**
*   Writing data to one of the available buffers
@data - pointer to data
@len - length of transmitted data
@bufn - buffer number to use
@addr - address(0-255) to buffer
@return eAt45status
*/
eAt45status at45_load_to_buff(const uint8_t * data,
                              const uint32_t len,
                              const uint8_t addr,
                              const uint8_t bufn)
{
  //--------------------comm | 16 dummy bits | 8 bit adress
  uint8_t opcode[4]  = {0x00, DUMMY8B, DUMMY8B, addr};

  if(len > 256U) return AT_45_COMM_ERR;           //max len buff 256 byte

  switch(bufn)
  {
    case BUFFER1: opcode[0] = LOAD_BUF1; break;   //load to buf1
    case BUFFER2: opcode[0] = LOAD_BUF2; break;   //load to buf2
    default: return AT_45_COMM_ERR; break;        //wrong operation
  }

  CS_RESET;

  if(AT_45_OK != spi_transmit(opcode, sizeof(opcode), MAX_BYTE_TIME * sizeof(opcode)))
    return AT_45_COMM_ERR;

 if(AT_45_OK != spi_transmit(data, len, MAX_BYTE_TIME * len))
   return AT_45_COMM_ERR;
 // if(AT_45_OK != spi_recive( (uint8_t *)data, len, MAX_BYTE_TIME * len))
 //   return AT_45_COMM_ERR;

  CS_SET;

  return AT_45_OK;
}


/**
*
Write buffer 1 or 2 to main memory page with or without erasure
@page - pahe number(0-32767)
@bufn - BUFFER1/BUFFER2(which buffer read)
@er - erase or not
@return eAt45status
*/
eAt45status at45_write_to_main(const uint16_t page,
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

    default: return AT_45_COMM_ERR; break;         //wrong operation

  }

  /* addr */
  com_page_addr[1] = (uint8_t)( (page & 0xFF00) >> 8);          //MSB
  com_page_addr[2] = (uint8_t)( (page & 0x00FF) >> 0);          //LSB

  /* Let's write the data from the buffer N, to the address(page_addr)s */
  CS_RESET;
  if(AT_45_OK != spi_transmit(com_page_addr, sizeof(com_page_addr), MAX_BYTE_TIME * sizeof(com_page_addr)))
    return AT_45_COMM_ERR;
  CS_SET;

  return AT_45_OK;
}



/**
*   Write to at45 page without erasing.
The page must be erased before writing.
The whole buffer is written.

@page - page number from 0 to 32767
@addr - from which byte to write to the buffer (0-255)
@src - pointer to the data being written
@length - length of transmitted data in bytes
@bufn - which buffer is being written to memory
@return - eAt45status
*/
eAt45status at45_page_write_noer(const uint16_t page,
                                 const uint8_t addr,
                                 const uint8_t * src,
                                 const uint32_t len,
                                 const uint8_t bufn)
{
  if( len > PAGE_SIZE || page > ALL_PAGES - 1)
    return AT_45_COMM_ERR;

  if(AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  /* send com. writes to the buffer and fill with data */
  if(at45_load_to_buff(src, len, addr, bufn)) return AT_45_COMM_ERR;

  /* Let's write the data from the buffer to the address page */
  if(at45_write_to_main(page, bufn, 0)) return AT_45_COMM_ERR;

  if(AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  return AT_45_OK;
}

/**
*   Write to at45 page with preliminary erasure.
*   the whole buffer is written
@page - page number from 0 to 32767
@addr - from which byte to write to the buffer (0-255)
@src - pointer to the data being written
@length - length of transmitted data in bytes
@bufn - which buffer is being written to memory
"return - eAt45status
*/
eAt45status at45_page_write_er(const uint16_t page,
                               const uint8_t addr,
                               const uint8_t * src,
                               const uint32_t len,
                               const uint8_t bufn)
{
  if( len > PAGE_SIZE || page > ALL_PAGES - 1) return AT_45_COMM_ERR;

  if(AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  /* send com. writes to the buffer and fill with data */
  if(at45_load_to_buff(src, len, addr, bufn)) return AT_45_COMM_ERR;

  /* Let's write the data from the buffer to the address page */
  if(at45_write_to_main(page, bufn, 1)) return AT_45_COMM_ERR;

  if(AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  return AT_45_OK;
}

/**
Function of writing two pages of memory
NOTE: Important!!! We write 2 pages!
To speed up writing and we will write 512 bytes, 256 in buffer1 and 256 in buffer2;
those. open the file, let's say some.txt is 61.3 KB (62,820 bytes)
62280 / 256(one page) = 245 entries, remainder 100 bytes;
but because we write on the page, instead of 100 bytes we will write 256, +256 bytes.
Total - 247 records.
We lose in memory usage, but simplify addressing.


TODO: Should be done in parallel
Not using now
@uint16_t page, uint8_t * src, uint32_t len
*/
eAt45status at45_2page_write(uint16_t page, uint8_t * src, uint32_t len)
{
  if(len > (PAGE_X_2) || page > ALL_PAGES - 1 )
    return AT_45_COMM_ERR;

  if(at45_page_write_noer(page, 0x00, src, len/2, BUFFER1))
    return AT_45_COMM_ERR;

  if(at45_page_write_noer(page + 1, 0x00, src + 256, len/2, BUFFER2))
    return AT_45_COMM_ERR;

  return AT_45_OK;
}



/**
*   Function to set the page size to 256 bytes
@opcode - SIZE_256 or SIZE_264
@return eAt45status
*/
eAt45status at45_set_ps(uint8_t opc)
{
  uint8_t  res;
  uint32_t opcode = 0x00802A3D | (opc << 24);

  if( (opc != SIZE_256) && (opc != SIZE_264) ) return AT_45_COMM_ERR;

  if(AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  CS_RESET;
  res = spi_transmit( (uint8_t *)&opcode, sizeof(opcode), 0xFFFFFFFFU);
  CS_SET;

  if(res != AT_45_OK) return AT_45_COMM_ERR;

  return AT_45_OK;
}

/**
*   Function to check the set page size no arguments

@return value SIZE_256/SIZE_264/AT45_ERR -in case of a problem with spi
*/
uint8_t at45_ps_read(void)
{
  eAt45status res1, res2;
  uint8_t result[2];
  uint8_t opcode = READ_SR;

  CS_RESET;
  res1 = spi_transmit(&opcode , sizeof(opcode), TIME_OUT_SPI);
  res2 = spi_recive(result, sizeof(result), TIME_OUT_SPI);
  //res2 = spi_transmit( (uint8_t *)(&result), sizeof(result), TIME_OUT_SPI);
  CS_SET;

  if((res1 != AT_45_OK) || (res2 != AT_45_OK)) return 0;
  else
  {
    if(result[0] & (1 << 0)) return SIZE_256;
    else return SIZE_264;
  }
}



/**
* Complete erasure at45 Important!!! from 80 to 208s on the datasheet.
@free_mem - pointer to free space in at45
@return - eAt45status
*/
eAt45status at45_full_erase(uint32_t * free_mem)
{
  uint32_t timeout = 0;
  uint8_t res;
  uint32_t opcode = FULL_ERASE;


  if(AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  CS_RESET;
  res = spi_transmit( (uint8_t *)&opcode, sizeof(opcode), 0xFFFFFFFFU );
  CS_SET;

  if(res != AT_45_OK)
    return AT_45_COMM_ERR;

  while (!(at45_read_status() & 0x80))
  {

#ifdef RTOS
    osDelay(1);
#else
    delay_ms(1);
#endif
    timeout++;

    if(timeout >= MAX_FULL_ERASE_TIME)
      return AT_45_COMM_ERR;

  }

  *free_mem = ALL_SIZE;

#ifdef DEBUG_AT45
  printf("at45 free memory = %d Mbyte\r\n", ALL_SIZE/ONE_MB);
#endif

  return AT_45_OK;
}

/**
*
Erase function
@page - erasable page(0-32768)
@eAt45status - return
*/
eAt45status at45_page_erase(uint16_t page)
{
  //uint8_t res;
  uint8_t cmd[4] = {
    PGERASE,
    (uint8_t)(page >> 8), //msb
    (uint8_t)(page)    ,  //lsb
    DUMMY8B
  };

  if( AT_45_OK != at45_ready()) return AT_45_COMM_ERR;


  if(page > ALL_PAGES - 1) return AT_45_COMM_ERR;

  CS_RESET;
  spi_transmit( cmd, sizeof(cmd), sizeof(cmd) * MAX_BYTE_TIME);
  CS_SET;

  if( AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  return AT_45_OK;
}



/*
*   restart function
@return eAt45status
*/
eAt45status at45_reset(void)
{
  uint32_t opcode = RESTART;

  if( AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  CS_RESET;
  spi_transmit( (uint8_t *)(&opcode), sizeof(opcode), TIME_OUT_SPI);
  CS_SET;

#ifdef DEBUG_AT45
  printf("Restart at45... \r\n");
#endif

  if( AT_45_OK != at45_ready()) return AT_45_COMM_ERR;

  return AT_45_OK;
}

/**
*   Ready check function
note: When writing or erasing, at45 will set the BUSY flag in the status register
@void
@return eAt45status
*/
eAt45status at45_ready(void)
{
  uint16_t timeout = 0;

  while (!(at45_read_status() & 0x0080))
  {
#ifdef RTOS
    osDelay(1);
#else
    delay_ms(1);
#endif
    timeout++;

    if(timeout >= 1000U) return AT_45_COMM_ERR;
  }

  return AT_45_OK;
}


/*
*   Low level functions
*/
eAt45status spi_transmit(const uint8_t * data, size_t size, uint32_t timeout)
{
  uint32_t tick = GetTick();
  uint32_t i = 0;
  volatile uint8_t tmp;

  /* check parameters */
  if(size == 0 || data == NULL || timeout == 0) return AT_45_ER_PAR;

  /* send data */
  while(i < size)
  {

    if(SPI1->SR & SPI_SR_TXE)
    {
      *((volatile uint8_t*)&(SPI1->DR)) = data[i];
      i++;
    }
    else
    {
      if((GetTick() - tick) >=  timeout)  return AT_45_SPI_TIMOUT;
    }
  }

  while(!(SPI1->SR & SPI_SR_TXE))
  {
    if((GetTick() - tick) >=  timeout) return AT_45_SPI_TIMOUT;
  }

  /* wait end operation */
  while(SPI1->SR & SPI_SR_BSY)
  {
    if((GetTick() - tick) >=  timeout) return AT_45_SPI_TIMOUT;
  }

  /* clear */
  tmp = SPI1->DR;
  tmp = SPI1->SR;

  return AT_45_OK;
}


/**
*   SPI data reading function. 8 bit implementation
*/
eAt45status spi_recive(uint8_t * rdata, size_t size, uint32_t timeout)
{
  volatile uint8_t tmp;         //for clear overrun flag
  uint32_t tick = GetTick();    //Tick++ in tim1
  uint32_t i = 0;

  if(rdata == NULL || size == 0 || timeout == 0)
    return AT_45_ER_PAR;

  /* read data */
  while(i < size)
  {
    /* wait end operation */
    while(SPI1->SR & SPI_SR_BSY)
    {
      if((GetTick() - tick) >=  timeout)
        return AT_45_SPI_BUSY;
    }

    if(SPI1->SR & SPI_SR_TXE)
      *(volatile uint8_t *)&SPI1->DR = (uint8_t)0x00;

    while(!(SPI1->SR & SPI_SR_RXNE))
    {
      if((GetTick() - tick) >=  timeout)
        return AT_45_SPI_TIMOUT;
    }

    rdata[i] = *(volatile uint8_t *)&SPI1->DR;
    i++;

    if((GetTick() - tick) >=  timeout)
      return AT_45_SPI_TIMOUT;
  }


  /* wait end operation */
  while(SPI1->SR & SPI_SR_BSY)
  {
    if((GetTick() - tick) >=  timeout)
      return AT_45_SPI_BUSY;
  }

  /* clear ovrr fl*/
  tmp = SPI1->DR;
  tmp = SPI1->SR;

  return AT_45_OK;
}
