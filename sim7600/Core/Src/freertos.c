#include "FreeRTOS.h"
#include "task.h"
#include "main.h"



/* Hook prototypes */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook (void);
void vAssertCalled( unsigned long ulLine, const char * const pcFileName );

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
  while(1);
}

void vApplicationMallocFailedHook(void)
{
 while(1);
}



