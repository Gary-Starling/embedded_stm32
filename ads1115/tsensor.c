

#include "main.h"
#include "math.h"



I2C_HandleTypeDef hi2c1;







#pragma optimize=none
int read_ads1115(void)
{
  
  uint8_t conf[3]={0x01,0x87,0x83};
  uint8_t read=0x00;
  uint8_t temp_byte[2];
  uint8_t err_bit=0;
  int16_t delta; 
  uint16_t err_count=0;


  if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)ADS1114_ADDR, (uint8_t*)&conf,3,10)!=HAL_OK)
    err_bit=1; //ошибка
  else
    err_bit=0; //успех

  
  
  while ((HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) && !err_bit)
  {
    HAL_IWDG_Refresh(&hiwdg);
    err_count++;
    if(err_count >= 15000)
    {
      err_count = 0;
      err_bit = 1;  //Таймаут
    }
  }
  
  err_count = 0;
  
  
  if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)ADS1114_ADDR, (uint8_t*)&read,1,10)!=HAL_OK)
    err_bit = 1; //ошибка
  else
    err_bit = 0; //успех
  
  
  while ((HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) && !err_bit)
  {
    HAL_IWDG_Refresh(&hiwdg);
    err_count++;
    if(err_count >= 15000)
    {
      err_count = 0;
      err_bit = 1;  //Таймаут
    }
  }
  
  err_count=0;
  

  if(HAL_I2C_Master_Receive(&hi2c1, (uint16_t)ADS1114_ADDR, (uint8_t*)&temp_byte,2,10)!=HAL_OK)
    err_bit = 1; //ошибка
  else
    err_bit = 0; //успех
  

  while ((HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) && !err_bit);
  {
    HAL_IWDG_Refresh(&hiwdg);
    err_count++;
    if(err_count >= 15000)
    {
      err_count = 0;
      err_bit = 1;  //Таймаут
    }
  }
  
  
  err_count=0;
  
  if(!err_bit)  
  {
    delta = temp_byte[0]<<8 | temp_byte[1];
    return delta;
  }
  else 
    return 20000;
}

void MX_I2C1_Init(void)
{
  
  
  __HAL_RCC_I2C1_CLK_ENABLE();  
  __HAL_RCC_I2C1_FORCE_RESET();
  __HAL_RCC_I2C1_RELEASE_RESET();

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  
  HAL_I2C_Init(&hi2c1);
  
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_I2C1_CLK_ENABLE();

  
}


