#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <Buttons_menu.h>

void segchar (char seg);
void ledprint(char* number, Menu_structure menu);
void sevenled_init(void);
void led_IT(void);



/* Знакоместо */
#define Dig1_Pin GPIO_PIN_8
#define Dig1_GPIO_Port GPIOA
#define Dig2_Pin GPIO_PIN_9
#define Dig2_GPIO_Port GPIOA
#define Dig3_Pin GPIO_PIN_10
#define Dig3_GPIO_Port GPIOA
#define Dig4_Pin GPIO_PIN_11
#define Dig4_GPIO_Port GPIOA
#define Dig5_Pin GPIO_PIN_12
#define Dig5_GPIO_Port GPIOA
#define Dig6_Pin GPIO_PIN_15
#define Dig6_GPIO_Port GPIOA

#define DIG1_SET  HAL_GPIO_WritePin(GPIOA, Dig1_Pin, GPIO_PIN_RESET)
#define DIG2_SET  HAL_GPIO_WritePin(GPIOA, Dig2_Pin, GPIO_PIN_RESET)
#define DIG3_SET  HAL_GPIO_WritePin(GPIOA, Dig3_Pin, GPIO_PIN_RESET)
#define DIG4_SET  HAL_GPIO_WritePin(GPIOA, Dig4_Pin, GPIO_PIN_RESET)
#define DIG5_SET  HAL_GPIO_WritePin(GPIOA, Dig5_Pin, GPIO_PIN_RESET)
#define DIG6_SET  HAL_GPIO_WritePin(GPIOA, Dig6_Pin, GPIO_PIN_RESET)

#define DIG1_RESET HAL_GPIO_WritePin(GPIOA, Dig1_Pin, GPIO_PIN_SET)
#define DIG2_RESET HAL_GPIO_WritePin(GPIOA, Dig2_Pin, GPIO_PIN_SET)
#define DIG3_RESET HAL_GPIO_WritePin(GPIOA, Dig3_Pin, GPIO_PIN_SET)
#define DIG4_RESET HAL_GPIO_WritePin(GPIOA, Dig4_Pin, GPIO_PIN_SET)
#define DIG5_RESET HAL_GPIO_WritePin(GPIOA, Dig5_Pin, GPIO_PIN_SET)
#define DIG6_RESET HAL_GPIO_WritePin(GPIOA, Dig6_Pin, GPIO_PIN_SET)

/* Сегменты */
#define SA GPIO_PIN_2
#define SB GPIO_PIN_0
#define SC GPIO_PIN_4
#define SD GPIO_PIN_12
#define SE GPIO_PIN_10
#define SF GPIO_PIN_1
#define SG GPIO_PIN_5
#define SH GPIO_PIN_3

#define SA_SET   HAL_GPIO_WritePin(GPIOB, SA, GPIO_PIN_RESET)
#define SB_SET   HAL_GPIO_WritePin(GPIOB, SB, GPIO_PIN_RESET)
#define SC_SET   HAL_GPIO_WritePin(GPIOB, SC, GPIO_PIN_RESET)
#define SD_SET   HAL_GPIO_WritePin(GPIOB, SD, GPIO_PIN_RESET)
#define SE_SET   HAL_GPIO_WritePin(GPIOB, SE, GPIO_PIN_RESET)
#define SF_SET   HAL_GPIO_WritePin(GPIOB, SF, GPIO_PIN_RESET)
#define SG_SET   HAL_GPIO_WritePin(GPIOB, SG, GPIO_PIN_RESET)
#define SH_SET   HAL_GPIO_WritePin(GPIOB, SH, GPIO_PIN_RESET)
#define SA_RESET HAL_GPIO_WritePin(GPIOB, SA, GPIO_PIN_SET)
#define SB_RESET HAL_GPIO_WritePin(GPIOB, SB, GPIO_PIN_SET)
#define SC_RESET HAL_GPIO_WritePin(GPIOB, SC, GPIO_PIN_SET)
#define SD_RESET HAL_GPIO_WritePin(GPIOB, SD, GPIO_PIN_SET)
#define SE_RESET HAL_GPIO_WritePin(GPIOB, SE, GPIO_PIN_SET)
#define SF_RESET HAL_GPIO_WritePin(GPIOB, SF, GPIO_PIN_SET)
#define SG_RESET HAL_GPIO_WritePin(GPIOB, SG, GPIO_PIN_SET)
#define SH_RESET HAL_GPIO_WritePin(GPIOB, SH, GPIO_PIN_SET)


#endif


