/* Модуль работы с кнопками и форматированным выводом меню */

#include <stm32f0xx_hal.h>
#include <Buttons_menu.h>
#include <stdbool.h>
#include <stdio.h>
#include <led.h>
#include <Flash_EEPROM.h>
#include <relay.h>

// TODO: Объекты кнопки пока сделаны глобально, что не есть хорошо, нужно будет передавть их по указателю, в функции обработчики
/*
  Внутри структуры есть флажки нажатия, которые используются в прерывании
  Пока что, мы избавились от тонны переменных, не структурированных, теперь есть общая структура
*/
static volatile Button_info left = {0}, right = {0}, press = {0}, save = {0}; //Объявим кнопки

volatile uint16_t zum_del; //Заряжает пищалку

/* Наши настройки глобальны */
extern int Ustanovki[N_UST];
extern const int maxval[N_UST];
extern const int minval[N_UST];
extern IWDG_HandleTypeDef hiwdg;

static char *dt_err_msg(Dt_states state);

/* Инит пинов кпопок */
void Buttons_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = Key_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Key_1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = Key_2_Pin | Key_4_Pin | Key_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/*
 buttons callback
 1 - <<
 2 - >>
 3 - P(menu)
 4 - S(menu go out)
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  switch (GPIO_Pin)
  {
  case Key_1_Pin: // <<
    left.press_flag = true;
    break;

  case Key_2_Pin: // >>
    right.press_flag = true;
    break;

  case Key_3_Pin: // P
    press.press_flag = true;
    break;

  case Key_4_Pin: // S
    save.press_flag = true;
    break;

  default:
    break;
  }
}

/**
 * @brief Функция собирает сообщение для вывода на смисегментник в зависимости от ситуации, длины температур и т.п.
 *
 * @param menu состояние меню (*a).b == a->b
 * @param t1 копии температур
 * @param t2
 * @param dt_err указатель на состояния датчиков
 * @param rel_state состояние реле
 */
void Menu_work_ind(Menu_structure *menu, int t1, int t2, Dt_states *dt_err, uint8_t rel_state)
{
  char string_out[7] = {0};
  static int t1_out = 0, t2_out = 0; //для редкого вывода

  switch (menu->mode)
  {
  case LIGHT_MENU: //Основной вывод

    if (menu->t_disp_cnt >= 150)
    {
      menu->t_disp_cnt = 0;
      t1_out = t1;
      t2_out = t2;
    }
    switch (menu->ind)
    {
    case SENS1: //Первый датчик
      if (dt_err[DT_1] != OK_T)
        sprintf(string_out, "1  %sq", dt_err_msg(dt_err[DT_1])); //Если какая либо ошибка
      else
        sprintf(string_out, "1%4dq", t1_out); //Обычный вывод
      break;
    //-------------------------------------------
    case SENS2: //Второй датчик
      if (dt_err[DT_2] != OK_T)
        sprintf(string_out, "2  %sq", dt_err_msg(dt_err[DT_2])); //Если какая либо ошибка
      else
        sprintf(string_out, "2%4dq", t2_out); //Обычный вывод
      break;
    //-------------------------------------------
    case SENS1_2:                                       //две температуры
      if (dt_err[DT_1] == OK_T && dt_err[DT_2] == OK_T) //Обе температуры
        sprintf(string_out, "%2dq%2dq", t1_out, t2_out);
      else if (dt_err[DT_1] == OK_T && dt_err[DT_2] != OK_T) //Ошибка датчика 2
        sprintf(string_out, "%2dq%s", t1_out, dt_err_msg(dt_err[DT_2]));
      else if (dt_err[DT_1] == !OK_T && dt_err[DT_2] == OK_T) //Ошибка датчика 1
        sprintf(string_out, "%s%dq", dt_err_msg(dt_err[DT_1]), t2_out);
      else if (dt_err[DT_1] != OK_T && dt_err[DT_2] != OK_T) //Оба в аварии
      {
        if (menu->mig < 64)
          sprintf(string_out, "%s%s", dt_err_msg(dt_err[DT_1]), dt_err_msg(dt_err[DT_2]));
        else
          sprintf(string_out, "      ");
      }
      break;
    //-------------------------------------------
    case RELAY_STATE: //состояние реле
      switch (rel_state)
      {
      case OFF_OFF:
        sprintf(string_out, "1oF2oF");
        break;
      case ON_OFF:
        sprintf(string_out, "1on2oF");
        break;
      case OFF_ON:
        sprintf(string_out, "1oF2on");
        break;
      case ON_ON:
        sprintf(string_out, "1on2on");
        break;
      }
      break;
      //-------------------------------------------
    case ERORR_IND: //Ошибка двух датчиков
      sprintf(string_out, "------");
      break;
    }
    break;

  case FAST_MENU: //Меню быстрой настройки
    switch (menu->state)
    {
    case 1:
      sprintf(string_out, "C1%4d", Ustanovki[menu->state]);
      break; //Установка температуры 1
    case 2:
      sprintf(string_out, "C2%4d", Ustanovki[menu->state]);
      break; //Установка температуры 2
    }
    break;

  case SETTINGS:             //Показ входа в инженерку, делается однократно
    menu->mode = LIGHT_MENU; //Обманка,чтобы не стирало в ledprint
    sprintf(string_out, "SEttin");
    ledprint(string_out, *menu); //Вывод
    for (int i = 0; i < 5; ++i)
    {
      my_delay(150);
      ZUMMER_ON;
      zum_del = 20;
    }
    my_delay(1000);
    sprintf(string_out, "      "); //очистим экран
    ledprint(string_out, *menu);
    menu->mode = INJ_MENU;
    menu->state = 3;
    return;
    break;

  case INJ_MENU: //Инженерное меню
    switch (menu->state)
    {
    case 3:
      sprintf(string_out, "h1 %3d", Ustanovki[menu->state]);
      break; //Гистерезис 1
    case 4:
      sprintf(string_out, "h2 %3d", Ustanovki[menu->state]);
      break; //Гистерезис 2
    case 5:
      sprintf(string_out, "1^%4d", Ustanovki[menu->state]);
      break; //Верхняя граница диапазона регулировки температуры канала 1
    case 6:
      sprintf(string_out, "1_%4d", Ustanovki[menu->state]);
      break; //нижняя граница диапазона регулировки температуры канала 1
    case 7:
      sprintf(string_out, "2^%4d", Ustanovki[menu->state]);
      break; //верхняя граница диапазона регулировки температуры канала 2
    case 8:
      sprintf(string_out, "2_%4d", Ustanovki[menu->state]);
      break; //нижняя граница диапазона регулировки температуры канала 2
    case 9:
      sprintf(string_out, "CC1%3d", Ustanovki[menu->state]);
      break; //корректировка температуры канала 1
    case 10:
      sprintf(string_out, "CC2%3d", Ustanovki[menu->state]);
      break; //корректировка точности канала 2
    case 11:
      sprintf(string_out, "in1%3d", Ustanovki[menu->state]);
      break; //инверсия реле 1
    case 12:
      sprintf(string_out, "in2%3d", Ustanovki[menu->state]);
      break; //инверсия реле 2
    }
    break;

  default: //Ошибка вывода
    break;
  }

  ledprint(string_out, *menu); //Вывод
}

/* Работает с глобальным Menu */
/**
 * @brief
 *
 * @param menu - структура
 * Теперь данный код исполняется в main.c поэтому мы можем сохранять установки прямо тут
 */
void button_handler(Menu_structure *menu)
{
  static bool push_fl = false;
  static uint8_t plus_item = 1;
  static uint8_t minus_item = 1;

  scan_push(menu, &minus_item, &plus_item);
  left_handler(menu, &push_fl, minus_item);
  right_handler(menu, &push_fl, plus_item);
  press_handler(menu, &push_fl);
  save_handler(menu, &push_fl);
  excess_settings(menu);
  restart_func(menu);

  if (push_fl) //Если было нажатие какой либо кнопки
  {
    push_fl = false;
    ZUMMER_ON;
    zum_del = 10;
    menu->exit_cnt = MENU_TIME;
  }

  if (menu->exit_cnt == 0 && menu->mode) //Выход автоматом,через 10 секунд
  {
    ZUMMER_ON;
    zum_del = 10;
    save.cnt = 0;
    save.press_flag = false;
    menu->mode = LIGHT_MENU;
    menu->state = SENS1;
    Write_ustanovki();
  }
}
//------------------------------------------------------------------------------

/**
 * @brief Считает задержки для кнопок
 *
 * @param menu
 * @param minus - вычитаемое
 * @param plus  - прибавляемое
 */
void scan_push(Menu_structure *menu, uint8_t *minus, uint8_t *plus)
{
  static uint8_t item_plus_count = 0, item_minus_count = 0;

  if (left.press_flag == true) //Однократное нажатие
  {
    if (Left_hold)
      left.cnt++;
    else
    {
      left.cnt = 0;
      left.press_flag = false;
    }
  }
  else //Нажатие было,но кнопку не отпустили
  {
    left.cnt = 0; //Чтобы не выполнять условие однократного нажатия
    if (Left_hold)
      left.count_hold++;
    else
    {
      left.count_hold = 0; //Всё-таки кнопка отжата
      left.hold_flag = false;
      item_minus_count = 0;
      *minus = 1;
    }

    if (left.count_hold >= 150) //держим кнопку долго, для автоприбавления
    {
      left.count_hold = 0;
      left.hold_flag = true;
      item_minus_count++;
    }

    /* Вычитаемое */
    if (item_minus_count >= 20)
      *minus = 5;
    if (item_minus_count >= 60)
      *minus = 10;
  }
  //Аналогия для right
  if (right.press_flag == true)
  {
    if (Right_hold)
      right.cnt++;
    else
    {
      right.cnt = 0;
      right.press_flag = false;
    }
  }
  else
  {
    right.cnt = 0;
    if (Right_hold)
      right.count_hold++;
    else
    {
      right.count_hold = 0; //Кнопка отжата
      right.hold_flag = false;
      item_plus_count = 0;
      *plus = 1;
    }

    if (right.count_hold >= 150) //Кнопку держат долго
    {
      right.count_hold = 0;
      right.hold_flag = true;
      item_plus_count++;
    }

    if (item_plus_count >= 20)
      *plus = 5;
    if (item_plus_count >= 60)
      *plus = 10;
  }
  //----------P-----BUTTON-------------------------------------------------
  if (press.press_flag == true)
  {
    if (press_hold)
      press.cnt++;
    else
    {
      press.cnt = 0;
      press.press_flag = false;
    }
  }
  else
  {
    press.cnt = 0;

    if (press_hold) //Нажатие кнопки P(кнопку держат)
    {
      press.count_hold++;

      if (press.count_hold % 500 == 0) //Чтобы периодически мигать и издавать писк
      {
        ZUMMER_ON;
        zum_del = 10;               //Завели счётчик для зумера
        menu->state++;              //Перелистнули индикацию
        menu->exit_cnt = MENU_TIME; //Обновили время нахождения в меню
        /* Если в пользовательском меню, циклически меняем 1-ю и 2-ю уставку */
        if (menu->mode == FAST_MENU) //
        {
          if (menu->state > 2) //
            menu->state = 1;
        }
        /* Если в инженерном меню, то всё остальное, с 3 по последню и по кругу*/
        if (menu->mode == INJ_MENU)
        {
          if (menu->state > END_MENU_STATE)
            menu->state = 3;
        }
      }
    }
    else
    {
      press.count_hold = 0; //Вдруг кнопку отжали
      press.hold_flag = false;
    }
    if (press.count_hold >= 5000) //Кнопку P держат долго, хотят зайти в инж.меню
    {
      press.count_hold = 0;
      press.hold_flag = true;
      menu->mode = SETTINGS;
    }
  }

  //-----------S---BUTTON-------------------------------------------------------
  if (save.press_flag == true) //
  {
    if (save_hold) //Считываем время нажатия
      save.cnt++;  //Если время нажатия > x Значит отработаем  событие в button_handler();
    else           //Кнопку отжали или дребезг
    {
      save.cnt = 0;
      save.press_flag = false;
    }
  }
  else
    save.cnt = 0; //Пока нет внешнего события,обнуляемся
}

//------------------------------------------------------------------------------
void left_handler(Menu_structure *menu, bool *push_fl, uint8_t item)
{
  static uint8_t minus_hold = 0;
  if (menu->state) //Работа в меню
  {
    if (left.cnt > BUTT_DEL) // BUTT_DEL можно изменять
    {
      left.cnt = 0;
      left.press_flag = false;
      *push_fl = true;

      if (Ustanovki[menu->state] <= minval[menu->state])
        Ustanovki[menu->state] = minval[menu->state];
      else
        Ustanovki[menu->state]--;
    }

    if (Left_hold && left.hold_flag && !Right_hold) //Кнопку зажали
    {
      if (Ustanovki[menu->state] <= minval[menu->state])
        Ustanovki[menu->state] = minval[menu->state];
      else
      {
        minus_hold++;
        if (minus_hold >= HOLD_DEL) //Автовычитание
        {
          *push_fl = true;
          minus_hold = 0;
          Ustanovki[menu->state] = Ustanovki[menu->state] - item;
        }
      }
    }
    else
      minus_hold = 0;
  }
  else
  { //Отображение параметров
    if (left.cnt > BUTT_DEL)
    {
      left.cnt = 0;
      left.press_flag = false;
      *push_fl = true;
      if (menu->ind == 0)
        menu->ind = 3;
      else
        menu->ind--;
    }
  }
}

//------------------------------------------------------------------------------
void right_handler(Menu_structure *menu, bool *push_fl, uint8_t item)
{
  static uint8_t plus_hold = 0;

  if (menu->state)
  {
    if (right.cnt > BUTT_DEL) //
    {
      right.cnt = 0;
      right.press_flag = false;
      *push_fl = true;

      if (Ustanovki[menu->state] >= maxval[menu->state])
        Ustanovki[menu->state] = maxval[menu->state];
      else
        Ustanovki[menu->state]++;
    }

    if (Right_hold && right.hold_flag && !Left_hold)
    {
      if (Ustanovki[menu->state] >= maxval[menu->state])
        Ustanovki[menu->state] = maxval[menu->state];
      else
      {
        plus_hold++;
        if (plus_hold >= HOLD_DEL)
        {
          *push_fl = true;
          plus_hold = 0;
          Ustanovki[menu->state] = Ustanovki[menu->state] + item;
        }
      }
    }
    else
      plus_hold = 0;
  }
  else
  {
    if (right.cnt > BUTT_DEL) //
    {
      right.cnt = 0;
      right.press_flag = false;
      *push_fl = true;
      if (menu->ind >= 3)
        menu->ind = 0;
      else
        menu->ind++;
    }
  }
}

//------------------------------------------------------------------------------
void press_handler(Menu_structure *menu, bool *push_fl)
{
  if (press.cnt > BUTT_DEL) //
  {
    *push_fl = true;
    press.cnt = 0;
    press.press_flag = false;

    switch (menu->mode)
    {
    case INJ_MENU:
      menu->exit_cnt = MENU_TIME;
      menu->state++;
      if (menu->state > N_UST - 1) //Если мы в инженерном меню,то двигаемся с 3 по N_UST пункт
        menu->state = 3;
      break;

    case FAST_MENU:
      menu->state++;
      menu->exit_cnt = MENU_TIME;
      if (menu->state > 2) //С1 и C2
        menu->state = 1;
      break;

    case LIGHT_MENU: //Меню вывода
      menu->mode = FAST_MENU;
      // menu->exit_cnt = MENU_TIME;
      menu->state++;
      break;
    }
  }
  else
  {
    if (menu->mode != 2 && press.hold_flag) //кнопку зажали,для входа в инж.меню
    {                                       // go to menu
      menu->mode = INJ_MENU;                //Вошли в инж. меню
      menu->state = 3;                      //Перелистнули окошко вывода
      menu->exit_cnt = MENU_TIME;
      ZUMMER_ON;
      zum_del = 10;
      press.hold_flag = 0;
      press.cnt = 0;
      press.press_flag = 0;
    }
  }
}
//------------------------------------------------------------------------------
void save_handler(Menu_structure *menu, bool *push_fl)
{
  if (save.cnt > BUTT_DEL)
  {
    if (menu->mode)
    {
      *push_fl = true;
      menu->exit_cnt = 0;
      save.cnt = 0;
      save.press_flag = false;
      menu->mode = LIGHT_MENU;
      menu->state = SENS1;
      Write_ustanovki();
    }
    else
    {
      *push_fl = true;
      save.press_flag = false;
      save.cnt = 0;
    }
  }
}

/* Функция возращает указатель на строку с ошибкой */
static char *dt_err_msg(Dt_states state)
{
  switch (state)
  {
  case ERROR_T:
    return "---";
    break;

  case HI_T:
    return "hi ";
    break;

  case LOW_T:
    return "Lo ";
    break;

  default:
    return "---";
    break;
  }
}

/**
 * @brief
 *
 * @param menu
 * Функция принимает на вход указатель на структуру menu
 * Далее проверяет по таблице пределы, т.к. мы не должны вылезти за минимум или максимум
 */
void excess_settings(Menu_structure *menu)
{
  switch (menu->state)
  {
  case 1: // ограничение температуры-1 сверху
    if (Ustanovki[menu->state] >= OGRUP1)
      Ustanovki[menu->state] = OGRUP1;
    //ограничение температуры-2 снизу
    if (Ustanovki[menu->state] < OGRDOWN1)
      Ustanovki[menu->state] = OGRDOWN1;
    break;
    //------------------------------------------------------------------------------
  case 2: // ограничение температуры-2 сверху
    if (Ustanovki[menu->state] >= OGRUP2)
      Ustanovki[menu->state] = OGRUP2;
    //ограничение температуры-2
    if (Ustanovki[menu->state] < OGRDOWN2)
      Ustanovki[menu->state] = OGRDOWN2;
    break;
    //------------------------------------------------------------------------------
  case 5: //верхний предел температуры-1 не может быть меньше нижнего
    if (Ustanovki[menu->state] < OGRDOWN1)
      Ustanovki[menu->state] = OGRDOWN1;
    break;
    //------------------------------------------------------------------------------
  case 6: //нижний предел температуры-1 не может быть больше верхнего
    if (Ustanovki[menu->state] >= OGRUP1)
      Ustanovki[menu->state] = OGRUP1;
    break;
    //------------------------------------------------------------------------------
  case 7: //верхний предел температуры-2 не может быть меньше нижнего
    if (Ustanovki[menu->state] < OGRDOWN2)
      Ustanovki[menu->state] = OGRDOWN2;
    break;
    //------------------------------------------------------------------------------
  case 8: //нижний предел температуры-1 не может быть больше верхнего
    if (Ustanovki[menu->state] >= OGRUP2)
      Ustanovki[menu->state] = OGRUP2;
    break;
    //------------------------------------------------------------------------------
  }
}

/**
 * @brief
 *
 * @param menu
 * Функция перезапуска всей системы и сброса к дефолтным настройкам
 */
void restart_func(Menu_structure *menu)
{
  char *res_msg = "RESEt\n";

  if (Left_hold && Right_hold && (menu->mode == LIGHT_MENU))
  {
    menu->restart_cnt++;
    if (menu->restart_cnt >= 10000)
    {
      ledprint(res_msg, *menu); //Вывод
      RELE1_RESET;              //На всякий случай выключим оба реле
      RELE2_RESET;
      for (int i = 0; i < 5; ++i)
      {
        my_delay(150);
        ZUMMER_ON;
        zum_del = 20;
      }
      EEPROM_erase();
      def_write();
      my_delay(200);
      HAL_NVIC_SystemReset();
    }
  }
  else
  {
    menu->restart_cnt = 0;
  }
}
/**
 * @brief Функция блокирующей задержк, сделана чтобы использовать вместо HAL_Delay
 * Внутри будет использоваться HAL_IWDG_Refresh, чтобы пребор не сбрасывался
 * 1 del = 1 mS
 * 
 * @param del 
 */

void my_delay(uint32_t del)
{
  for(uint32_t i = 0; i < del; i++)
  {
    for(uint32_t j = 0; j < 1360; j++)
    __NOP();
    HAL_IWDG_Refresh(&hiwdg);
  }
}
