
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"


void Error_Handler(void);


#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA
#define B1_EXTI_IRQn EXTI0_IRQn
#define CS_SPI_Pin GPIO_PIN_4
#define CS_SPI_GPIO_Port GPIOC
#define W5500_INT_Pin GPIO_PIN_5
#define W5500_INT_GPIO_Port GPIOC
#define W5500_INT_EXTI_IRQn EXTI9_5_IRQn
#define W5500_RESET_Pin GPIO_PIN_0
#define W5500_RESET_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define LD4_Pin GPIO_PIN_12
#define LD4_GPIO_Port GPIOD
#define LD3_Pin GPIO_PIN_13
#define LD3_GPIO_Port GPIOD
#define LD5_Pin GPIO_PIN_14
#define LD5_GPIO_Port GPIOD
#define LD6_Pin GPIO_PIN_15
#define LD6_GPIO_Port GPIOD


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

