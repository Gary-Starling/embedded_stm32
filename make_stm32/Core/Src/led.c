
#include <led.h>
#include <stm32f0xx_hal.h>
#include <stdio.h>
#include <string.h>


char R1=' ',R2=' ',R3=' ',R4=' ',R5 = ' ',R6 = ' ';

uint8_t leng = 0;
uint16_t menu_mig = 0;
char string_out[6];

uint16_t Segments[8] = {GPIO_PIN_3, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_2, GPIO_PIN_10, GPIO_PIN_12, GPIO_PIN_5, GPIO_PIN_4};
uint16_t Digits[6] =   {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_15};


//===============================

void sevenled_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  
  HAL_GPIO_WritePin(GPIOB, SA|SB|SC|SD|SC|SE|SF|SH, GPIO_PIN_RESET);
  
  HAL_GPIO_WritePin(GPIOA, Dig1_Pin|Dig2_Pin|Dig3_Pin|Dig4_Pin|Dig5_Pin|Dig6_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = SA|SB|SC|SD|SE|SF|SG|SH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  

  GPIO_InitStruct.Pin = Dig1_Pin|Dig2_Pin|Dig3_Pin|Dig4_Pin|Dig5_Pin|Dig6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//all symbol 
//TODO: писать в порт , без исп-я функций 
void segchar (char seg)
{
  switch(seg)
  {
  case '0':
    SA_SET;SB_SET;SC_SET;SD_SET;SE_SET;SF_SET;SG_RESET;SH_RESET;
    break;
  case '1':
    SA_RESET;SB_SET;SC_SET;SD_RESET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;
  case '2':
    SA_SET;SB_SET;SC_RESET;SD_SET;SE_SET;SF_RESET;SG_SET;SH_RESET;
    break;
  case '3':
    SA_SET;SB_SET;SC_SET;SD_SET;SE_RESET;SF_RESET;SG_SET;SH_RESET;
    break;
  case '4':
    SA_RESET;SB_SET;SC_SET;SD_RESET;SE_RESET;SF_SET;SG_SET;SH_RESET;
    break;
  case '5':
    SA_SET;SB_RESET;SC_SET;SD_SET;SE_RESET;SF_SET;SG_SET;SH_RESET;
    break;
  case '6':
    SA_SET;SB_RESET;SC_SET;SD_SET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;
  case '7':
    SA_SET;SB_SET;SC_SET;SD_RESET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;
  case '8':
    SA_SET;SB_SET;SC_SET;SD_SET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;
  case '9':
    SA_SET;SB_SET;SC_SET;SD_SET;SE_RESET;SF_SET;SG_SET;SH_RESET;
    break;
  case ' '://space
    SA_RESET;SB_RESET;SC_RESET;SD_RESET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;
  case 'A':
    SA_SET;SB_SET;SC_SET;SD_RESET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;
  case 'B':
    SA_RESET;SB_RESET;SC_SET;SD_SET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;
  case 'C':
    SA_SET;SB_RESET;SC_RESET;SD_SET;SE_SET;SF_SET;SG_RESET;SH_RESET;
    break;
  case 'd':
    SA_RESET;SB_SET;SC_SET;SD_SET;SE_SET;SF_RESET;SG_SET;SH_RESET;
    break;
  case 'U':
    SA_RESET;SB_SET;SC_SET;SD_SET;SE_SET;SF_SET;SG_RESET;SH_RESET;
    break;
  case 'u':
    SA_RESET;SB_RESET;SC_SET;SD_SET;SE_SET;SF_RESET;SG_RESET;SH_RESET;
    break;
  case 'E':
    SA_SET;SB_RESET;SC_RESET;SD_SET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;
  case 'R':
    SA_RESET;SB_RESET;SC_RESET;SD_RESET;SE_SET;SF_RESET;SG_SET;SH_RESET;
    break;
  case 'L':
    SA_RESET;SB_RESET;SC_RESET;SD_SET;SE_SET;SF_SET;SG_RESET;SH_RESET;
    break;
  case 'o':
    SA_RESET;SB_RESET;SC_SET;SD_SET;SE_SET;SF_RESET;SG_SET;SH_RESET;
    break;
  case 'i':
    SA_RESET;SB_RESET;SC_SET;SD_RESET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;
  case 'I':
    SA_RESET;SB_SET;SC_SET;SD_RESET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;
  case 'h':
    SA_RESET;SB_RESET;SC_SET;SD_RESET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;	
  case 'P':
    SA_SET;SB_SET;SC_RESET;SD_RESET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;	
  case 'F':
    SA_SET;SB_RESET;SC_RESET;SD_RESET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;	
  case 'S':
    SA_SET;SB_RESET;SC_SET;SD_SET;SE_RESET;SF_SET;SG_SET;SH_RESET;
    break;
  case 't':
    SA_RESET;SB_RESET;SC_RESET;SD_SET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;	
  case 'j':
    SA_RESET;SB_SET;SC_SET;SD_SET;SE_SET;SF_SET;SG_SET;SH_RESET;
    break;		
  case 'q':
    SA_SET;SB_SET;SC_RESET;SD_RESET;SE_RESET;SF_SET;SG_SET;SH_RESET;
    break;
  case 'z':
    SA_SET;SB_SET;SC_RESET;SD_SET;SE_SET;SF_RESET;SG_SET;SH_RESET;
    break;	
  case '=':
    SA_RESET;SB_RESET;SC_RESET;SD_SET;SE_RESET;SF_RESET;SG_SET;SH_RESET;
    break;	
  case '_':
    SA_RESET;SB_RESET;SC_RESET;SD_SET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;	
  case '-':
    SA_RESET;SB_RESET;SC_RESET;SD_RESET;SE_RESET;SF_RESET;SG_SET;SH_RESET;
    break;
  case 'n':
    SA_RESET;SB_RESET;SC_SET;SD_RESET;SE_SET;SF_RESET;SG_SET;SH_RESET;
    break;
  case '^':
    SA_SET;SB_RESET;SC_RESET;SD_RESET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;    
  case 'Y':
    SA_RESET;SB_SET;SC_SET;SD_SET;SE_RESET;SF_SET;SG_SET;SH_RESET;
    break; 
    
  default:
    SA_RESET;SB_RESET;SC_RESET;SD_RESET;SE_RESET;SF_RESET;SG_RESET;SH_RESET;
    break;			
  }
}

/* 1.Всегда строго 6 символов чтобы не вылететь за указатель.
   2.Чтобы не плодить трёхэтажные условия */
void ledprint(char* number, Menu_structure menu)
{	
  uint8_t leng = 0;
  
  leng = (uint8_t)strlen(number);
  
  if(leng > 6 )
    leng = 6;
  
  if(leng < 6)
    return;

  if(menu.mode == 0)
  {
      R1 = number[0];
      R2 = number[1];
      R3 = number[2];
      R4 = number[3];
      R5 = number[4];
      R6 = number[5];
  } 
  else
  {
    if(menu.mig < 64)
    {
      if(menu.state >= 1 && menu.state <= 12)
      {
        R1 = number[0];
        R2 = number[1];
        R3 = number[2];
        R4 = ' ';
        R5 = ' ';
        R6 = ' ';
      }
      
    }
    else
    {
      R1 = number[0];
      R2 = number[1];
      R3 = number[2];
      R4 = number[3];
      R5 = number[4];
      R6 = number[5];
    }
  }
}

/* 300Hz */
void led_IT(void)
{
  static uint8_t n_count = 6;
  
  n_count--;
  
  switch(n_count)
  {
    /* биты устанвливаем, транзистор закрываем, pnp 
    т.е. горит только одно окошко, но при частоте 300Гц, нам этого не видно*/
  case 5:
    GPIOA->ODR |= (Dig2_Pin | Dig3_Pin | Dig4_Pin | Dig5_Pin | Dig6_Pin); 
    segchar(R1);
    GPIOA->ODR &= ~Dig1_Pin;
    break;
  case 4:
    GPIOA->ODR |= (Dig1_Pin | Dig3_Pin | Dig4_Pin | Dig5_Pin | Dig6_Pin); 
    segchar(R2);
    GPIOA->ODR &= ~Dig2_Pin;
    break;
  case 3:
    GPIOA->ODR |= (Dig1_Pin | Dig2_Pin | Dig4_Pin | Dig5_Pin | Dig6_Pin); 
    segchar(R3);
    GPIOA->ODR &= ~Dig3_Pin;
    break;
  case 2:
    GPIOA->ODR |= (Dig1_Pin | Dig2_Pin | Dig3_Pin | Dig5_Pin | Dig6_Pin); 
    segchar(R4);
    GPIOA->ODR &= ~Dig4_Pin;
    break;
  case 1:
    GPIOA->ODR |= (Dig1_Pin | Dig2_Pin | Dig3_Pin | Dig4_Pin | Dig6_Pin); 
    segchar(R5);
    GPIOA->ODR &= ~Dig5_Pin;
    break;
  case 0:
    GPIOA->ODR |= (Dig1_Pin | Dig2_Pin | Dig3_Pin | Dig4_Pin | Dig5_Pin); 
    segchar(R6);
    GPIOA->ODR &= ~Dig6_Pin;
    break;
  default:
    break;
  }

  
  if(n_count == 0) 
    n_count = 6;
  
}



