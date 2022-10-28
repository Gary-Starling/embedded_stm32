#include "stm32f0xx.h"
#include <stdio.h>
#include "4_20.h"
#include "adc.h"
#include "gpio.h"

extern volatile uint16_t adc_value; //from adc.c

void RCC_Init(void);


int main(void) {
  
  float tens = 0.0;     //Переменная давления
  
  RCC_Init();
  Gpio_Init();
  ADC1_DMA1_Init();
  
  
  while (1)  /* Основной цикл */
  {
    tens = get_tens_4_20(&adc_value);
    printf("pressure = %.3f bar\n", tens);
  }
}


void RCC_Init(void) {
  /* Вкл. HSI (8 МГц) */
  RCC_HSICmd(ENABLE); 
  /* Проверка готовности */
  while( RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) {
    //тут можно установить счётчик с таймаутом
  }
  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); // HSI как системна частота
  RCC_HCLKConfig(RCC_SYSCLK_Div1);       
  RCC_PCLKConfig(RCC_HCLK_Div1);     
  
  /* тактирование на порты */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE); 
  /* DMA */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  /* ADC */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  
  
}