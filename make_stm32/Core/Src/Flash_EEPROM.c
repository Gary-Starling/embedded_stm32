#include <Flash_EEPROM.h>
#include <stm32f0xx_hal.h>
                                    //      жук      C1    C2   h1   h2   1-    1_    2-    2_  cc1  cc2    in1  in2
                                    //        0      1     2    3    4    5     6     7     8     9   10    11   12
int Ustanovki[N_UST] =                  { 0xA5A5,   25,   24,   2,   2,  140,  -40,  140,  -40,   0,   0,   0,   0}; 
const int maxval[N_UST] =               { 0xA5A5,  140, 140,   30,  30,  140,  140,  140,  140,  20,  20,   1,   1};
const int minval[N_UST] =               { 0xA5A5,  -40, -40,    1,   1,  -40,  -40,  -40,  -40, -20, -20,   0,   0};
const int default_val[N_UST] =          { 0xA5A5,   25,  25,    2,   2,  140,  -40,  140,  -40,   0,   0,   0,   0}; 



void EEPROM_erase(void)
{
  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t PageError = 0;
  HAL_FLASH_Unlock();
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_START_ADDR;
  EraseInitStruct.NbPages = (FLASH_END_ADDR - FLASH_START_ADDR)/FLASH_PAGE_SIZE;
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {while(1);}
  HAL_FLASH_Lock();	
}


void Write_ustanovki(void)
{
  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t Address = FLASH_START_ADDR;
  uint32_t PageError = 0;
  HAL_FLASH_Unlock();
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_START_ADDR;
  EraseInitStruct.NbPages = (FLASH_END_ADDR - FLASH_START_ADDR)/FLASH_PAGE_SIZE;
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {while(1);}
  
  for(uint8_t i = 0; i < N_UST; ++i)
  {
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Ustanovki[i]) == HAL_OK) {Address +=4;}
    else {while(1);}
  }		
  HAL_FLASH_Lock();	
}

void Read_ustanovki(void)
{
  uint32_t Address = FLASH_START_ADDR;
  for(uint8_t i = 0; i < N_UST; ++i)
  {
    Ustanovki[i] = *(__IO uint32_t*) Address;
    Address += 4;
  }
}

void EEPROM_checking(void)
{
  if(*(__IO uint32_t*) FLASH_START_ADDR != 0xA5A5)
  {
    EEPROM_erase();
    Ustanovki[0] = 0xA5A5;
    Write_ustanovki();
  }
  else
    Read_ustanovki();
}

void def_write(void)
{
  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t Address = FLASH_START_ADDR;
  uint32_t PageError = 0;
  HAL_FLASH_Unlock();
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_START_ADDR;
  EraseInitStruct.NbPages = (FLASH_END_ADDR - FLASH_START_ADDR)/FLASH_PAGE_SIZE;
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) 
    while(1);
  
  for(uint8_t i = 0; i < N_UST; ++i)
  {
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, default_val[i]) == HAL_OK)
      Address +=4;
    else 
      while(1);
  }		
  HAL_FLASH_Lock();	
}



