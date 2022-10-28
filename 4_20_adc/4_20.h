/*stm32f030f4p6*/
/*
0-15 бар 4-20мА 
1mA = 1B
R = 150 Om
Токовая петля
*/


#ifndef _4_20_H_
#define _4_20_H_

#define MIN_V                   (0.0)
#define MAX_V                   (3.3)

float adc_to_sens(int adc);
float get_tens_4_20(volatile uint16_t *adc_adr);
void user_del(uint32_t cnt);

#endif