#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <kty_81_110.h>
#include <stdbool.h>

/* Перечисление состояний вывода меню */
enum MENU_MODE
{
  LIGHT_MENU = 0, //Основная индикация
  FAST_MENU = 1,  //Меню "юзер-настроек", можно изменить 1,2 температуру
  SETTINGS = 2,   //Сообщение которое показывает вход в инж.меню
  INJ_MENU = 3    //Инженерное меню
};

/* Различные типы индикаци при нажатии кнопок << >> */
enum IND_MODE
{
  SENS1 = 0,
  SENS2 = 1,
  SENS1_2 = 2,
  RELAY_STATE = 3,
  ERORR_IND = 4
};

/* Индикации состояния реле*/
enum IND_RELAY
{
  OFF_OFF = 0x00,
  ON_OFF = 0x01,
  OFF_ON = 0x02,
  ON_ON = 0x03
};

/* Структура меню */
typedef struct menu_str
{
  uint32_t exit_cnt;    //Счётчик декр., для автовыхода из меню
  uint32_t mig;         //Для мигания сообщениями
  uint32_t restart_cnt; //Счётчик перезапуска
  uint8_t t_disp_cnt;   //Счётчик отображения температуры(для плавного отображения)
  uint8_t mode;         //Режимы индикации, вывод, меню и т.п.
  uint8_t state;        //Переменная подпунктов инженерного меню
  uint8_t ind;          //Переменная окошек индикации
} Menu_structure;

/* Структура кнопки */
typedef struct Button_param
{
  bool press_flag;    //Флаг однократного нажатия
  bool hold_flag;     //Флаг того, что кнопка зажата
  uint16_t cnt;       //Счётчик обработки дребезга
  uint16_t count_hold;//Счётчик зажатой кнопки
  void (*butt_handler)(Menu_structure *menu, bool *push_fl, uint8_t item);//Тут хорошо бы сделать указатель на функцию для кнопки
} Button_info;

void Buttons_Init(void);
void Menu_work_ind(Menu_structure *menu, int t1, int t2, Dt_states *dt_err, uint8_t rel_state);
void button_handler(Menu_structure *menu);
void scan_push(Menu_structure *menu, uint8_t *minus, uint8_t *plus);
void left_handler(Menu_structure *menu, bool *push_fl, uint8_t item);
void right_handler(Menu_structure *menu, bool *push_fl, uint8_t item);
void press_handler(Menu_structure *menu, bool *push_fl);
void save_handler(Menu_structure *menu, bool *push_fl);
void excess_settings(Menu_structure *menu);
void restart_func(Menu_structure *menu);
void my_delay(uint32_t del);

/* Defines */
#define ZUMMER_Pin GPIO_PIN_1
#define ZUMMER_GPIO_Port GPIOA

#define ZUMMER_ON HAL_GPIO_WritePin(ZUMMER_GPIO_Port, ZUMMER_Pin, GPIO_PIN_SET)
#define ZUMMER_OFF HAL_GPIO_WritePin(ZUMMER_GPIO_Port, ZUMMER_Pin, GPIO_PIN_RESET)

#define Key_1_Pin GPIO_PIN_7
#define Key_1_GPIO_Port GPIOA
#define Key_1_EXTI_IRQn EXTI4_15_IRQn
#define Key_2_Pin GPIO_PIN_11
#define Key_2_GPIO_Port GPIOB
#define Key_2_EXTI_IRQn EXTI4_15_IRQn
#define Key_4_Pin GPIO_PIN_8
#define Key_4_GPIO_Port GPIOB
#define Key_4_EXTI_IRQn EXTI4_15_IRQn
#define Key_3_Pin GPIO_PIN_9
#define Key_3_GPIO_Port GPIOB
#define Key_3_EXTI_IRQn EXTI4_15_IRQn

#define Left_hold HAL_GPIO_ReadPin(Key_1_GPIO_Port, Key_1_Pin) == GPIO_PIN_RESET
#define Right_hold HAL_GPIO_ReadPin(Key_2_GPIO_Port, Key_2_Pin) == GPIO_PIN_RESET
#define press_hold HAL_GPIO_ReadPin(Key_3_GPIO_Port, Key_3_Pin) == GPIO_PIN_RESET
#define save_hold HAL_GPIO_ReadPin(Key_4_GPIO_Port, Key_4_Pin) == GPIO_PIN_RESET

#define BUTT_DEL 60
#define MENU_TIME 5000

#define END_MENU_STATE 12
#define HOLD_DEL 100
/* Defines */

#endif
