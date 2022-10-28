#include <stdint.h>
#include <stm32f030x6.h>
#include <stdio.h>
#include "main.h"


int main(void);

#define GPIO_PIN_8 ((uint16_t)0x0100U) /* Pin 8 */

/* externs */

/* externs */

int Test_Var = 123;
float global_var = 0.0f;
const char *some_text = "Hello Embedded\n";
const int x_var = 100;

char msg[50] = "\n";

int var_a;
int var_b = 0;
int var_c = 0x1234;

int main(void)
{
    HAL_Init();
    
    var_a++;
    var_b--;
    var_c = 0;
    uint32_t var = 0;

    /* Настройка PA8 */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //Разрешаем тактировать GPIOA на шине APB2

    GPIOA->MODER &= ~(GPIO_MODER_MODER8);               // установим 00 в в MODERx[1:0] - сбросим
    GPIOA->MODER |= GPIO_MODER_MODER8_0;                // установим 01, настроим как выход
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_8;                 // установим 1 в OTx, настроим как выход
    GPIOA->OSPEEDR |= (0x3UL << GPIO_PUPDR_PUPDR8_Pos); // в OSPEEDRy[1:0]: установим 11, т.е high speed
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_1;                // PUPDRx[1:0]: установис 10, Pullup

    while (1)
    {
        Test_Var++;
        global_var += 0.1;

        var++;
        if (var >= 1000)
        {
            var = 4;
            GPIOA->ODR ^= GPIO_PIN_8;
            sprintf(msg, "%s", "Test message\n");
            printf(msg);
        }
    }
}

void Reset_Handler(void)
{
    /* startup */
    extern uint8_t _sdata, _edata, _sidata,
        __bss_start__, __bss_end__; 

    uint8_t *dst = &__bss_start__;

    //Обнуление bss 
    while (dst < &__bss_end__)
     *dst++ = 0;

    dst = &_sdata;
    //запись в .data данных из flash
    uint8_t *src = &_sidata;
    
    while (dst < &_edata)
     *dst++ = *src++;

    
    main();
    
}
