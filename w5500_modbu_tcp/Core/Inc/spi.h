

#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"



extern SPI_HandleTypeDef hspi1;



HAL_StatusTypeDef MX_SPI1_Init(void);
void HAL_SPI1_deInit(void);



#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */


