
#include "stm32f0xx.h"
#include "4_20.h"
#include <stdio.h> //для printf


float OutTab[]= {0.0, 1.0, 2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0 , 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
float InTab[] = {739, 925, 1110, 1295, 1481, 1666, 1851, 2037, 2222, 2407, 2593,  2779, 2963, 3149, 3335, 3520, 3703};



float get_tens_4_20(volatile uint16_t *adc_adr) {
  uint32_t temp = 0;
  float R = 150.0;
  float current = 0.0;
  
  for(int i = 0; i<10; i++) 
  {
    temp += *adc_adr;
    user_del(1000); //тут можно использовать os/hal delay или самописный
  }
  
  temp /= 10;
  
  current = (float)((((float)(temp) * ((float)(MAX_V - MIN_V)/4096.0) + MIN_V))/R);
  /* Это для наглядности */
  printf("I = %.3fA\nI = %d mA\n", current, (int)(current * 1000));
  /* Вернём значение в барах */
  return  adc_to_sens(temp);
}


//------------------------------------------------------------------------------
float adc_to_sens(int adc)//----
{  
  uint8_t cnt = 0;   
  
  /* Возращает ошибку, для значений за пределами */ 
  if ( adc < 535 ) return  -1;
  if ( adc > 3685 ) return 1000;

  while ( InTab[++cnt] < adc );
  
  if(cnt) --cnt;
  return ( (double)OutTab[cnt]+((long)OutTab[cnt+1]-OutTab[cnt])*(adc-InTab[cnt])/
          (InTab[cnt+1]-InTab[cnt]) );
}

void user_del(uint32_t cnt) {
  int x = 0;
  
  for(uint32_t i = 0; i < cnt; i++)
    x++;
}