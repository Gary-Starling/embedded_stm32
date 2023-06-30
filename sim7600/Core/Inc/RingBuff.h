#ifndef _RING_BUFF_
#define _RING_BUFF_

/* Интерефейс кольцевого буфера для UART */

#include "stdint.h"
#include "stddef.h"
#include "stm32f405xx.h"
#include <stdbool.h>

/* Настройки */
#define _DMA_           (0U)    //Выбор способа работы с UART
#define _IRQ_           (1U)    //
#define RING_BUFF_LEN	(1024U) //Размер буфера
#define METHOD          (_DMA_) //_DMA_ or _INTERRUPT_
typedef uint8_t data_type;      //тип данных в буфере
/* Настройки */

typedef struct {
  volatile  const data_type * head;                 //Указатель на "головной" элемент
  volatile  const data_type * tail;                 //Хвост
  volatile  const data_type * prvt;                 //Элемент перед хвостом
  volatile  data_type   data[RING_BUFF_LEN];        //Буфер
}sRingBuff;

/* Функция заполняет нулями весь буфер и устанавливает указаетели в исходное положение */
void InitRingBuff(sRingBuff * buff);

/* Наличие данных в буфере */
size_t UnreadDataBuff(const sRingBuff * buff);

/* Запись данных в буфер */
#if METHOD == _IRQ_
void PutRingData(sRingBuff * buff, data_type data);
#elif METHOD == _DMA_
void PutRingData(sRingBuff * buff);
#endif

/* Чтение данных из буфера */
data_type ReadRingData(sRingBuff * buff);

/* Старт приёма по dma uart rx circular mode */
void RingRxDmaStart(sRingBuff * buff);
void RingRxDmaStop(void);
#endif