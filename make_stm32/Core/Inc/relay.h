#ifndef RELAY_H
#define RELAY_H

#include <stdint.h>
#include <stdbool.h>

#define RELE_GPIO_Port (GPIOA)

#define RELE1_Pin   (GPIO_PIN_4)
#define RELE2_Pin   (GPIO_PIN_0)

#define RELE1_SET   (HAL_GPIO_WritePin(RELE_GPIO_Port, RELE1_Pin, GPIO_PIN_SET))
#define RELE1_RESET (HAL_GPIO_WritePin(RELE_GPIO_Port, RELE1_Pin, GPIO_PIN_RESET))

#define RELE2_SET   (HAL_GPIO_WritePin(RELE_GPIO_Port, RELE2_Pin, GPIO_PIN_SET))
#define RELE2_RESET (HAL_GPIO_WritePin(RELE_GPIO_Port, RELE2_Pin, GPIO_PIN_RESET))

#define RELE1_READ   (HAL_GPIO_ReadPin(RELE_GPIO_Port, RELE1_Pin))
#define RELE2_READ   (HAL_GPIO_ReadPin(RELE_GPIO_Port, RELE2_Pin))

void Relay_init(void);
void Relay_work(int real_temp, int set_temp, int hyst, int invers, unsigned short rel_pin);

#endif