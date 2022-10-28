
#include <main.h>
#include <stm32f0xx_it.h>
#include <led.h>

extern TIM_HandleTypeDef htim3;//, htim14;
extern volatile uint16_t zum_del;

void NMI_Handler(void)
{

  while (1)
    ;
}

void HardFault_Handler(void)
{

  while (1)
    ;
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

/* Таймер для индикации около 300 Гц */
void TIM3_IRQHandler(void)
{
  extern Menu_structure Menu;

  /* счётик для мигания сообщениями */
  if (Menu.mig >= 128)
    Menu.mig= 0;
  else
    Menu.mig++;
  /* Инкрементный счётчик для отображения температуры */
  Menu.t_disp_cnt++;
  /* счётик писка зумера */
  if (zum_del != 0)
    zum_del--;
  else
    ZUMMER_OFF;
  /* Автоиндекрементный счётчик времени нахождени в меню*/
  if (Menu.exit_cnt != 0)
    Menu.exit_cnt--;
  /* Отображение на семсегментнике */
  led_IT();
  HAL_TIM_IRQHandler(&htim3);
}


// /* Для кноопок и пищалки около 600Гц */
// void TIM14_IRQHandler(void)
// {
//   HAL_TIM_IRQHandler(&htim14);
// }

void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}