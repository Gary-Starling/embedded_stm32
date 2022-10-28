

#include "at45db641e.h"
#include "stdbool.h"

SPI_HandleTypeDef hspi1;


extern void Error_Handler(void);


uint32_t memory_size = ALL_SIZE;
uint16_t page_write = WRITE_ADT_ST;   //с какой страницы начинаем запись
//Вообще это хранилка адреса, что, куда и когда писали
//Её тоже надобно хранить
//в целях теста по записи пары файлов,объявим её пока так


void SPI2_Init(void)
{
   
   SPI_GPIO_Init();
   
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


void SPI_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /*Configure GPIO pin Output Level */
  CS_RESET;
  
  GPIO_InitStruct.Pin = CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CS_PORT, &GPIO_InitStruct);
  /* Не забудь добавить свои пины mosi, miso,
  и подать тактирование */
  
}

/*---------------------------------------------------------------------------*/
uint8_t ad45test(void) 
{
  
  HAL_StatusTypeDef result_1, result_2;
  
  // read the device id
  
  uint8_t opcode[1] = { 0x9F };
  uint8_t devid_res[5];
  
  CS_RESET;
  result_1 = HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), 500);
  result_2 = HAL_SPI_Receive(&hspi1, devid_res, sizeof(devid_res), 500);
  CS_SET;
  
  if((result_1 != HAL_OK) || (result_2 != HAL_OK)) 
  {
#ifdef DEBUG
    printf("Error during getting the device id, res1 = %d, res2 = %d\r\n", result_1, result_2);
#endif
    return ERR;
  }
  else
  {  
#ifdef DEBUG
    printf("Manufacturer ID: 0x%02X\r\n"
           "Device ID (byte 1): 0x%02X\r\n"
             "Device ID (byte 2): 0x%02X\r\n"
               "Extended device information (EDI) string length: 0x%02X\r\n"
                 "EDI byte 1: 0x%02X\r\n"
                   "--------\r\n",
                   devid_res[0], devid_res[1], devid_res[2],
                   devid_res[3], devid_res[4]);
#endif
    return OK;
  }
}
/*---------------------------------------------------------------------------*/
uint8_t at45_read_status(void)
{
  
  uint8_t opcode[1] = { 0xD7 };
  uint8_t result[1];
  
  CS_RESET;
  HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), 500);
  HAL_SPI_Receive(&hspi1, result, sizeof(result), 500);
  CS_SET;
  return result[0];
}
/*---------------------------------------------------------------------------*/

/* 
uint16_t page – номер читаемой страницы (0 – 32767) у нас 16мб памяти
uint16_t addr – адрес байта внутри страницы (0 – 255) 256 бит страница
uint32_t length – количество читаемых бит
uint8_t *out – указатель, куда класть прочитанные данные
*/
uint8_t at45_read_data(uint16_t page, uint16_t addr, uint32_t length, uint8_t *out)
{
  register  uint8_t res;
  register  uint32_t timeout=0;  
  register  uint32_t map_addr;
  //uint8_t  state;      //rd   adr  adr  adr  magick byte
  uint8_t  opcode[5] = { 0x0B,0x00,0x00,0x00,0x00 };/* 0x0b- Чтение*/
  //uint8_t comm[1] = { 0xFE };
  
  if(page>=32768) //За границей страниц памяти
    return ERR;
  //Ожидаем готовности
  do 
  {
    res = at45_read_status();
    timeout++;
    
    if(timeout>=1000)
      return ERR; 
  } 
  while (!(res & 0x80)); 
  //Формируем адрес для чтения 
  map_addr = ((page << 8) | (addr & 0xFF));
  
  /* Тут можно дописать универсальность под обе страницы памяти
  if (state & 0x01) //256
  else //264
  map_addr = ((page << 10) | (addr & 0x3FF));//тут тоже допилить
  */
  //Формируем адрес читаемой страницы,а читать будем с нуля
  //opcode[0] = (uint8_t)(0x0B);
  opcode[1] = (uint8_t)(opcode[1] | (map_addr >> 16));
  opcode[2] = (uint8_t)(opcode[2] | (map_addr>> 8));
  opcode[3] = (uint8_t)(map_addr);
  //opcode[4] = (uint8_t)(0x00); //Магический байти
  
  CS_RESET;
  
  HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), HAL_MAX_DELAY);
  
  for (int j = 0; j < length; j++)
    HAL_SPI_Receive(&hspi1, out + j, 1, HAL_MAX_DELAY);
  
  CS_SET;
  
  return OK;
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
uint8_t at45_page_write(uint16_t page, uint8_t *data, uint16_t length)
{
  uint8_t res;
  uint32_t timeout=0;                         
  //buff|1dum 15addr|8dummy|)
  uint8_t opcode[4]  = {0x84,0x00,0x00,0x00}; //com-записи в буфер1 
  //Мы начинаем для удобства с нулевого адреса
  uint8_t command[1] = {0x88}; //комманда - Записать содержиемое буф. в память
  //без затирания, при первой инициализации мы будем полностью стирать
  //микру, потом в неё писать,так намного быстрее
  uint8_t page_addr[3] = {0x00,0x00,0x00};
  //uint8_t result; 
  
  if(length>256) //Данные больше чем страница памяти
    return ERR;
  
  if(page>=32768) //Максимумальное количество страниц
    return ERR;
  
  //формируем адрес исходя из размера страницы
  page_addr[0] = (uint8_t)(page >> 8);
  page_addr[1] =  ((uint8_t)((page & 0xFF)));
  
  
  CS_RESET;//отпправляем ком. запис в буфер
  HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), HAL_MAX_DELAY);
  
  for (int i = 0; i < length; i++) //Набиваем буфер даннымм
  {
    HAL_SPI_Transmit(&hspi1, data+i, 1, HAL_MAX_DELAY);
  }
  CS_SET;
  
  CS_RESET; //Отправляем 0x83(Запись в буффер) по адресу в основной памяти
  HAL_SPI_Transmit(&hspi1, command, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, page_addr, sizeof(page_addr), HAL_MAX_DELAY); 
  CS_SET;
  
  
  do 
  {
    res = at45_read_status();
    timeout++;
    
    if(timeout>=1000)
      return ERR; 
  } 
  while (!(res & 0x80)); 
  
  return OK;
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
void at45_set_512(void) //Задать размер 512
{
  uint8_t  res;
  uint8_t opcode[4] = {0x3d,0x2a,0x80,0xa6};
  
  do 
  {
    res = at45_read_status();
  } 
  while (!(res & 0x80));  
  
  CS_RESET;
  HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), HAL_MAX_DELAY);
  CS_SET;
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
void at45_full_erase(void)
{
  uint8_t  res;
  uint8_t opcode[4] = {0xc7,0x94,0x80,0x9a};
  
  do 
  {
    res = at45_read_status();
  } 
  while (!(res & 0x80));  
  
  
  CS_RESET;
  HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), HAL_MAX_DELAY);
  CS_SET;
  memory_size=ALL_SIZE;
  
  
  do 
  {
    res = at45_read_status();
  } 
  while (!(res & 0x80)); 
  
#ifdef DEBUG
  printf("at45 free memory = %d Mbyte\r\n", memory_size/ONE_MB);
#endif
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
uint8_t at45_reset(void)
{
  uint8_t  res;
  uint8_t  opcode[4] = {0xf0,0x00,0x00,0x00};//Комманда рестарта
  uint32_t timeout=0;
  
  do //Готова ли микросхема к ресесту?
  {
    res = at45_read_status();
    timeout++;
    
    if(timeout>=1000)
      return ERR; 
  } 
  while (!(res & 0x80));  
  
  
  CS_RESET;
  HAL_SPI_Transmit(&hspi1, opcode, sizeof(opcode), HAL_MAX_DELAY);
  CS_SET;
#ifdef DEBUG
  printf("Restart at45... \r\n");
#endif
  do //Ждём окончания рестарта
  {
    res = at45_read_status();
    timeout++;
    
    if(timeout>=1000)
      return ERR; 
  } 
  while (!(res & 0x80));  
  
  return OK; 
  
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*
uint8_t write_file(char* file_name, InfoFile_str* fill)
{
  
  uint32_t bytesread=0;
  int32_t  size;
  uint8_t buff_write[256]={0};
  
  if(f_open(&WavFile,file_name,FA_OPEN_EXISTING | FA_READ) == FR_OK)
  {
    size = WavFile.obj.objsize-44;
    strcpy (fill->name, file_name);
    fill->size = WavFile.obj.objsize-44;
    fill->adr =  page_write;
    
    
    f_lseek(&WavFile, 44); //В начало файла 
	
    while(size!=0) //или page>32767
    { //читаем кусочек
      if(f_read (&WavFile, &buff_write[0], 256, &bytesread)!=FR_OK)
      {
        Error_Handler();
        f_close(&WavFile);
        return ERR;
      }     				
					
      //Пишем кусочек
      if(at45_page_write(page_write, buff_write, 256)!=OK)
      {
        Error_Handler();
        f_close(&WavFile);
        return ERR;
      }
      
      page_write++;
      size = size - bytesread;
      
      if(size<=0)
        size=0;
    }
    
    f_close(&WavFile);
    memory_size -= WavFile.obj.objsize; 
#ifdef DEBUG
    printf("at45 free memory = %d Kb, \r\n", memory_size/ONE_MB); 
#endif
    return OK;
  }
  else
    return ERR;
}
*/
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
//Читать целый кусок файла 
/*
uint16_t at45_read_part(uint16_t pg, uint32_t size, uint8_t* reciv)
{ 
  register uint16_t iter=0;
  register uint16_t step=0; 
  
  iter = size/256; //вычислим количество чтений
  
  do 
  {
    if(at45_read_data(pg, 0, 256, reciv+(step*PAGE_READ))==OK)
    {
      pg++;
      step++;
    }
    else
    {
      AudioPlay_Stop();
      return ERR;
    }
    
  } while (step<iter); //
  
  return pg; //Возвращаем кол-во прочитанных страниц
}
*/