#include <main.h>
#include <stm32f0xx_hal.h>

extern DMA_HandleTypeDef hdma_adc;

void HAL_MspInit(void)
{

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{
  if (htim_base->Instance == TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
  else if (htim_base->Instance == TIM14)
  {
    __HAL_RCC_TIM14_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM14_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM14_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
  //
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (hadc->Instance == ADC1)
  {
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*
    ADC GPIO Configuration
    PA5     ------> ADC_IN5
    PA6     ------> ADC_IN6
    */

    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC Init */
    hdma_adc.Instance = DMA1_Channel1;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    hdma_adc.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_adc) != HAL_OK)
    {
      while (1)
        ;
    }

    __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    __HAL_RCC_ADC1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5 | GPIO_PIN_6);

    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
}
