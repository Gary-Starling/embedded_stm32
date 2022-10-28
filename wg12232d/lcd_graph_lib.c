#include "stm32f0xx_hal.h"
#include "lcd_graph_lib.h"
#include "font_6x8.h"       
#include "Font_12x16.h"
#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>
#include <cmsis_os2.h>


/* Массивы под знаоместа */
unsigned char PlaceArray[] = {120,114,108,102,96,90,84,78,72,66,60,54,48,42,36,30,24,18,12,6}; 
unsigned char PlaceArray_x12[] = {122,110,98,86,74,62,50,38,26,14};


/** Функция инициалицации порта на вход или выход.
    Сделана для приёма данных или отправки.

    state - INPUT или OUTPUT
    
    noreturn
*/
void LCDG_InitPort(eDataPortInit state)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0}; 
  
  GPIO_InitStruct.Pin = DB0_Pin|DB1_Pin|DB2_Pin|DB3_Pin|DB4_Pin|DB5_Pin|DB6_Pin|DB7_Pin;
  
  if(state == OUTPUT)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  else
  {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

/** Функция очистки порта

    state - CLEAR (очищает нужные нам пины)
            SETUP (устанавливает)
    
    noreturn
*/
void data_port_set(eDataPortState state) 
{  
  if (state == CLEAR)
    GPIOB->ODR &= 0xF807;
  else
    GPIOB->ODR &= 0x07F8;
}

/** Функция для проверки флага занятости
  Возможна ситуация когда при отправке данных или команды, чип дисплея 
  будет занят выполнением операций, поэтому следует дождаться снятия флага.

  noinput
  noreturn
*/
void check_busy(void)
{
  uint16_t busy_f = 0x00;
  
  LCDG_InitPort(INPUT);
  
#ifdef __FREE_RTOS__  
  taskENTER_CRITICAL();
#endif
  A0_RESET; 
  RW_SET; 
  
  //TODO: счётчик на выход из while
  do
  {
    strobe_off(CS1);
    busy_f = (GPIOB->IDR);
    strobe_on(CS1);
    
    strobe_off(CS2);
    busy_f = (GPIOB->IDR);
    strobe_on(CS2);
  }while(busy_f&0x0400); 
  
  strobe_off(BOTH);
  RW_RESET;
#ifdef __FREE_RTOS__  
  taskEXIT_CRITICAL();
#endif
}


/** Функция чтения данных из дисплея 

  return - возращает прочитанный байт
*/
//TODO: нужно сделать передачу координат x,y, дальнейшее чтение автоинкрементируется
uint8_t LCDG_ReadData(void)
{
  uint16_t data = 0x00;
  
  LCDG_InitPort(INPUT);
#ifdef __FREE_RTOS__ 
  taskENTER_CRITICAL();
#endif  
  A0_SET;                           
  RW_SET;	
  
  strobe_on(CS1);		
  nop();			
  strobe_off(CS1);
  nop();
  strobe_on(CS1);			
  data = GPIOB->IDR;		
  strobe_off(CS1);		
  
  A0_RESET;    
#ifdef __FREE_RTOS__   
  taskEXIT_CRITICAL();
#endif    
  return((uint8_t)(data>>3)); //т.к. так расположены пины 
}





/** Отпрвка команды дисплею 
    data - команда(? может так и назвать ?)
    chip - номер чипа

    noreturn
*/

void LCDG_SendCom(uint8_t data, eCSnumer chip)
{
  /* Упаковка данных для передачи в порт */
  uint16_t data_on_port = 0x0000 | (data << 3); 
  
  check_busy();
  
  LCDG_InitPort(OUTPUT);
#ifdef __FREE_RTOS__    
  taskENTER_CRITICAL();
#endif   
  data_port_set(CLEAR);
  
  A0_RESET;                         //A0 = 0 command 
  RW_RESET;
  
  GPIOB->ODR |= data_on_port;       //Запишем
  
  strobe_on(chip);
  nop();
  strobe_off(chip);
  nop();
  RW_RESET;
#ifdef __FREE_RTOS__     
  taskEXIT_CRITICAL();
#endif   
}


/** Отправка данных  в дисплей 
    data - данные
    chip - номер чипа 

    noreturn
*/
void LCDG_SendData(uint8_t data, eCSnumer chip)
{
  /* Упаковка данных для передачи в порт */
  uint16_t data_on_port = 0x0000 | (data << 3); 
  
  check_busy();
  
  LCDG_InitPort(OUTPUT);
#ifdef __FREE_RTOS__      
  taskENTER_CRITICAL(); 
#endif     
  data_port_set(CLEAR);
  
  A0_SET;                       //  A0 = 1 data
  RW_RESET;
  
  GPIOB->ODR |= data_on_port;     
  strobe_on(chip);
  nop();
  nop();
  strobe_off(chip);
  
  A0_RESET;  
#ifdef __FREE_RTOS__       
  taskEXIT_CRITICAL();
#endif
}

/** Инициализация дисплея
    noinput
    noreturn
*/
void LCDG_InitLcd(void)
{
  
  data_port_set(CLEAR);
  DISPLAY_ON;                   //Включение дисплея(my custom board)
#ifdef __FREE_RTOS__      
  osDelay(100);      
#else
  HAL_Delay(100);
#endif  
  
  LCDG_SendCom(COM_RESET, BOTH); 
  LCDG_SendCom(COM_ADC_SELECT_ON, BOTH);
  LCDG_SendCom(COM_CLEAR_RMW, BOTH);
  LCDG_SendCom(COM_STATIC_DRIVE_OFF, BOTH);
  LCDG_SendCom(COM_DUTY_SELECT_ON, BOTH); 
  LCDG_SendCom(COM_CLEAR_RMW, BOTH);
  LCDG_SendCom(COM_DISPLAY_ON, BOTH);
}


/** Функция заполняющая дисплей данными от x1 до x2
    x1,x2 - координаты от и до 
    data - данные
    x1 = 0  y = 20 data = 0xff
     ==========================
    |#######                   |
    |#######                   |
    |#######                   |
    |#######                   |
     ==========================
*/
void LCDG_FillAll(unsigned int x1, unsigned int x2, uint8_t data)
{
  uint32_t xPos = x1;
  uint32_t realAdr = 0;
  eCSnumer chip;
  
  while(xPos < x2)
  {
    if (realAdr < 61) 
    {
      chip = CS2;
      realAdr = realAdr + 19;
    }
    else 
    {
      chip = CS1;
      realAdr = realAdr - 42;
      LCDG_SendCom(COM_START_LINE(0), CS1); 
    }
    
    LCDG_SendCom(PAGE0,chip);
    LCDG_SendCom(realAdr,chip);
    LCDG_SendData(data,chip);    
    LCDG_SendCom(PAGE1,chip);
    LCDG_SendCom(realAdr,chip);  
    LCDG_SendData(data,chip);
    LCDG_SendCom(PAGE2,chip);
    LCDG_SendCom(realAdr,chip);
    LCDG_SendData(data,chip);
    LCDG_SendCom(PAGE3,chip);
    LCDG_SendCom(realAdr,chip);
    LCDG_SendData(data,chip);
    
    xPos++;
    realAdr = xPos;  
  } 
}

/** Отправка символа на дисплей 
    uint8_t xPos, uint8_t yPos  - координаты (122x32)
    char symbol - символ        
    eInversion inv - установить или включить инверсию
*/
void LCDG_SendSymbol(uint8_t xPos, uint8_t yPos, char symbol, eInversion inv)
{
  uint32_t realAdr;
  uint32_t realPage = PAGE0 + yPos;
  uint32_t  pointerFont = symbol*6;
  realAdr = PlaceArray[xPos];
  xPos = realAdr;
  eCSnumer chip;
  
  for(int i = 0; i <= 5; i++)
  {
    if (realAdr < 61) 
    {
      chip = CS2;
      realAdr = realAdr + 19;
    }
    else 
    {
      chip = CS1;
      realAdr = realAdr - 61;
      realAdr = realAdr + 19;
    }
    
    symbol = font_6x8_Data[pointerFont];
    
    if(inv == ON)
      symbol = ~symbol;
    
    LCDG_SendCom(realPage, chip);
    LCDG_SendCom(realAdr, chip);
    LCDG_SendData(symbol, chip);
    xPos--;
    realAdr = xPos;
    pointerFont++;
    
  }
}

/** Отправка символа_x12 на дисплей  
    uint8_t xPos, uint8_t yPos  - координаты (122x32)
    char symbol - символ        
    eInversion inv - установить или включить инверсию
*/
void LCDG_SendSymbol_x12(uint8_t xPos, uint8_t yPos, char symbol,  eInversion inv)
{
  uint32_t realAdr2;
  uint32_t realAdr;
  uint32_t realPage = PAGE0 + yPos;
  uint32_t  pointerFont = symbol*24;
  realAdr = PlaceArray_x12[xPos];
  realAdr2 = PlaceArray_x12[xPos];
  xPos = realAdr;
  eCSnumer chip;
  
  for(int i = 0; i <= 11; i++)
  {
    if (realAdr < 61) 
    {
      chip = CS2;
      realAdr = realAdr + 19;
    }
    else 
    {
      chip = CS1;
      realAdr = realAdr - 61;
      realAdr = realAdr + 19;
    }
    symbol = Font_12x16_Data[pointerFont];
    
    if(inv == ON)
      symbol = ~symbol;
    
    LCDG_SendCom(realPage, chip);
    LCDG_SendCom(realAdr, chip);
    LCDG_SendData(symbol, chip);
    xPos--;
    realAdr = xPos;
    pointerFont++;
  }
  xPos = realAdr2;
  realPage++;
  
  for(int i = 0; i <= 11; i++)
  {
    if (realAdr2 < 61) 
    {
      chip = CS2;
      realAdr2 = realAdr2 + 19;
    }
    else 
    {
      chip = CS1;
      realAdr2 = realAdr2 - 61;
      realAdr2 = realAdr2 + 19;
    }
    
    symbol = Font_12x16_Data[pointerFont];
    
    if(inv == ON)
      symbol = ~symbol;
    
    LCDG_SendCom(realPage,chip);
    LCDG_SendCom(realAdr2,chip);
    LCDG_SendData(symbol,chip);
    xPos--;
    realAdr2 = xPos;
    pointerFont++;
  }
}



/** Отпавка строки на дисплей
    uint8_t xPos, uint8_t yPos  - координаты (122x32)
    char * string - строка на отправку        
    eInversion inv - установить или включить инверсию
*/
void LCDG_SendString(uint8_t xPos, uint8_t yPos, char * string, eInversion inv)
{
  while(*string)
  {
    LCDG_SendSymbol(xPos, yPos, *string++, inv);
    xPos++;
  }
}


/** Отпавка строки на дисплей_x12
    uint8_t xPos, uint8_t yPos  - координаты (122x32)
    char * string - строка на отправку        
    eInversion inv - установить или включить инверсию
*/
void LCDG_SendString_x12(uint8_t xPos, uint8_t yPos, char * string, eInversion inv)
{
  while(*string)
  {
    LCDG_SendSymbol_x12(xPos, yPos, *string++, inv);
    xPos++;
  }
}

/** Функцция включения стробирующего импульса
    eCSnumer chip - CS1/CS2/BOTH
*/
void strobe_on(eCSnumer chip)  
{
  switch(chip)
  {
  case 1: CS1_RESET; break;
  case 2: CS2_RESET; break;
  case 3: CS1_RESET; CS2_RESET; break;
  default://error 
    break;
  }
}

/** Функцция выключения стробирующего импульса
    eCSnumer chip - CS1/CS2/BOTH
*/
void strobe_off(eCSnumer chip)  
{
  switch(chip)
  {
  case 1: CS1_SET; break;
  case 2: CS2_SET; break;
  case 3: CS1_SET; CS2_SET; break;
  default://error 
    break;
  }
}

/* no operation finction */
void nop(void)
{
  for(int i = 0; i < 1; i++)
    __NOP();
}
