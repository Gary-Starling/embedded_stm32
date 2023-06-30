#include "RingBuff.h"

#pragma optimize=none


void RingRxDmaStop(void)
{
  /* DMA Disable Receiver */
  USART3->CR3 &= ~USART_CR3_DMAR;
  /* clear ore flag sequence below */
  USART3->SR;
  USART3->DR;
  /* Disable the Peripheral */
  DMA1_Stream1->CR &= ~DMA_SxCR_EN;
    /* Disable dma Transfer error/Direct mode error */
  DMA1_Stream1->CR  &= ~(((uint32_t)DMA_SxCR_TEIE) | ((uint32_t)DMA_SxCR_DMEIE)); //rx
  DMA1_Stream3->CR  &= ~(((uint32_t)DMA_SxCR_TEIE) | ((uint32_t)DMA_SxCR_DMEIE)); //tx
    /* Clear DBM bit */
  DMA1_Stream1->CR &= (uint32_t)(~DMA_SxCR_DBM);
}

/**
Запус приёма uart через dma, с включением  irq - DMA_IT_TE/DMA_IT_DME
sRingBuff * buff - указатель на кольцевой буфер
noreturn
*/
void RingRxDmaStart(sRingBuff * buff)
{
  InitRingBuff(buff);
  /* DMA Peripheral to Memory */
  /* Clear DBM bit */
  DMA1_Stream1->CR &= (uint32_t)(~DMA_SxCR_DBM);
  /* Configure DMA Stream data length */
  DMA1_Stream1->NDTR = sizeof(buff->data);
  /* Configure DMA Stream source address */
  DMA1_Stream1->PAR = (uint32_t)&USART3->DR;
  /* Configure DMA Stream destination address */
  DMA1_Stream1->M0AR = (uint32_t)&buff->data;
  /* Enable dma Transfer error/Direct mode error */
  DMA1_Stream1->CR  |= ((uint32_t)DMA_SxCR_TEIE) | ((uint32_t)DMA_SxCR_DMEIE); //rx
  DMA1_Stream3->CR  |= ((uint32_t)DMA_SxCR_TEIE) | ((uint32_t)DMA_SxCR_DMEIE); //tx
  /* Enable the Peripheral */
  DMA1_Stream1->CR |= DMA_SxCR_EN;
  /* clear ore flag sequence below */
  USART3->SR;
  USART3->DR;
  /* DMA Enable Receiver */
  USART3->CR3 |= USART_CR3_DMAR;
}

static void ClearRingBuff(sRingBuff * buff);

/**
Очистка буфера, установка указателей в исходное положение
sRingBuff * buff - указатель на используемую структуру данных
noreturn
*/
void InitRingBuff(sRingBuff * buff)
{
  ClearRingBuff(buff);
  buff->head =  buff->data;   //Поставим указатели в начало буфера
  buff->tail = buff->head + 1;
}


/**
Очистка буфера
sRingBuff * buff - указатель на используемую структуру данных
noreturn
*/
static void ClearRingBuff(sRingBuff * buff)
{
  for(size_t i = 0; i < (sizeof(buff->data)/sizeof(data_type)); i++)
    buff->data[i] = 0x00;
}

/**
Функция чтения количества непрочитанных данных
sRingBuff * buff - указатель на используемую структуру данных
return uint32_t - количество непрочитанных данных
*/
size_t UnreadDataBuff(const sRingBuff * buff)
{
  return (size_t)((RING_BUFF_LEN + buff->tail - buff->head) %  RING_BUFF_LEN);
}


/* Если используем побайтный приём */
#if METHOD == _IRQ_
/** Вставка данных в кольцевой буфер (При вставке перемещает ptr Tail)
sRingBuff * buff - указатель на используемую структуру данных
*/
void PutRingData(sRingBuff * buff, data_type data)
{
  data_type * tmp;
  /* Защита от переполнения, забегание хвоста за голову */
  if( UnreadDataBuff(buff) >= (RING_BUFF_LEN - 1) )
    return;

  tmp = (data_type *)buff->tail;
  buff->tail++;
  *(tmp) = data;

  if((buff->tail) > &(buff->data[RING_BUFF_LEN - 1])) //Если указываем на адрес за пределом буфера
    buff->tail = &(buff->data[0]);                    //Перейдём к началу

}
#elif METHOD == _DMA_
/* dma + idle_it
note: idle может сработать и в середине сообщения поэтому нам нужен какой-либо
символ для конца сообщения '\n' '\0' и т.п.
*/
void PutRingData(sRingBuff * buff)
{
  buff->tail = &buff->data[RING_BUFF_LEN  - DMA1_Stream1->NDTR];       //елемент в хвосте
  buff->prvt = &(buff->data[RING_BUFF_LEN  - DMA1_Stream1->NDTR]) - 1; //элемент перед хвостом

  /* защита за выход "перед" массивом */
  if(buff->prvt < &buff->data[0])
    buff->prvt = &(buff->data[RING_BUFF_LEN - 1]);

  /* Защита от переполнения, забегание хвоста за голову */
  if( UnreadDataBuff(buff) >= (RING_BUFF_LEN - 1) )
    buff->head = buff->tail;
}
#endif


/**
Чтение данных из буфера (При чтении перемещается ptr Head)
sRingBuff * buff - указатель на используемую структуру данных
return data_type - прочитанный элемент
*/
data_type ReadRingData(sRingBuff * buff)
{
  data_type data = *(buff->head);                       //Прочтём данные

  buff->head++;                                         //Передвинем указатель
  if((buff->head) > &(buff->data[RING_BUFF_LEN - 1]))   //Перейдём к началу
    buff->head = &(buff->data[0]);

  return data;
}