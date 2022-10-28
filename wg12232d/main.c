#include "lcd_graph_lib.h"
#include "main.h" 



int main()
{ 
  sys_config();
  pins_init();        
  LCDG_InitLcd();  
  LCDG_ClrAllDisp;    
  while(1)  
  {
    LCDG_SendString(0,0,"Hello my friend", OFF);
    LCDG_SendString(0, 1, "wg12232d ;)" , OFF);
    LCDG_SendString_x12(2, 2, "Abc" , OFF);
  }
}


void pins_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOC_CLK_ENABLE(); 
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE(); 
  
  /* display */
  HAL_GPIO_WritePin(GPIOB, A0_Pin|CS1_Pin|CS2_Pin|DB7_Pin|RW_Pin|POWER_DISP_PIN
                    |DB0_Pin|DB1_Pin|DB2_Pin|DB3_Pin|DB4_Pin|DB5_Pin|DB6_Pin, GPIO_PIN_RESET);
  
  GPIO_InitStruct.Pin = A0_Pin|CS1_Pin|CS2_Pin|DB7_Pin|RW_Pin|POWER_DISP_PIN|
    DB0_Pin|DB1_Pin|DB2_Pin|DB3_Pin|DB4_Pin|DB5_Pin|DB6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* display */
}




void sys_config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;   
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);


}









