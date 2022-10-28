#include "stm32f0xx.h"
#include "adc.h"

volatile uint16_t adc_value = 0; //Значение ацп

void ADC1_DMA1_Init(void) {
  
  /* структуры для иницта adc dma */
  ADC_InitTypeDef     ADC_InitStructure;
  DMA_InitTypeDef     DMA_InitStructure;
  
  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);
  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward; 
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  
  ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_239_5Cycles);  
  
  ADC_GetCalibrationFactor(ADC1);
  ADC_Cmd(ADC1, ENABLE);     
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
  
  /* DMA1 conf*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);  
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_value; 
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1; 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular); 
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
  
}