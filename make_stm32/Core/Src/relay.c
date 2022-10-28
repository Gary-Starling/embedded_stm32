#include <relay.h>
#include <stm32f0xx_hal.h>

/* Модуль работы с реле на универсальной плате, для DIN корпуса D4MG
   Пины A4, A0 */

void Relay_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = RELE1_Pin | RELE2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(RELE_GPIO_Port, &GPIO_InitStruct);
  RELE2_RESET;
  RELE1_RESET;
}

/*
  Функция управления реле
  real_temp  - передаваемая измеренная температура
  set_temp   - установленная температура
  hyst       - гистерезис включения/выключения реле
  invers     - инверсия реле
  rel_number - номер реле(т.е. пин т.к у нас define)
*/

void Relay_work(int real_temp, int set_temp, int hyst, int invers, unsigned short rel_pin)
{
  int t_on = set_temp - (hyst / 2);
  int t_off = set_temp + (hyst - (hyst / 2));

  if (real_temp <= t_on)
  {
    if (!invers)
      HAL_GPIO_WritePin(RELE_GPIO_Port, rel_pin, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(RELE_GPIO_Port, rel_pin, GPIO_PIN_RESET);
  }
  else if (real_temp >= t_off)
  {
    if (!invers)
      HAL_GPIO_WritePin(RELE_GPIO_Port, rel_pin, GPIO_PIN_RESET);
    else
      HAL_GPIO_WritePin(RELE_GPIO_Port, rel_pin, GPIO_PIN_SET);
  }
}