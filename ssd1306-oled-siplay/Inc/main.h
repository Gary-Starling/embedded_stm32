
#ifndef __MAIN_H
#define __MAIN_H

//#define  DEBUG
#define RELEASE  

#ifdef __cplusplus
extern "C" {
#endif
  
  /* Includes */
#include "stm32f0xx_hal.h"
#include "ssd1306.h"

  /* Includes */
  
  /*macro*/
  /*macro*/ 
  
  
  void Error_Handler(void);
  uint8_t Out_number_center(int num, char* str, uint8_t y);
    
    
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


