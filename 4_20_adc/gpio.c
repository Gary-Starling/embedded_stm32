#include "stm32f0xx.h"
#include "gpio.h"

void Gpio_Init(void) {
  
  GPIO_InitTypeDef gpio_str = {0};
  
  gpio_str.GPIO_Pin   = TENS_PIN;
  gpio_str.GPIO_Mode = GPIO_Mode_AN;
  gpio_str.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(TENS_PORT, &gpio_str);
}