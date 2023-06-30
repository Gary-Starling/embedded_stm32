#include "dtmf.h"
//#include "speaker.h"
#include <string.h>

/**
    const sExtraMsg * pExtra - указатель на полученный dtmf сигнал
    const char * pass -  указатель на пароль
    bool * pass_fl - флаг введённого пароля
    bool flush - для очистки
    noreturn
*/
void dtmfPass(const sExtraMsg * pDtmf, const char * pass, bool * pass_fl, bool flush)
{
  static char dtmf_sym[5] = {0};         //Строка dtmf символов
  static uint8_t dtmf_cnt = 0;           //Счётчик для dtmf сиволов
  static uint8_t attempt = 0;
  //sExtraMsg Extra;              //

  /* Очистка */
  if(flush)
  {
    dtmf_cnt = 0;
    attempt = 0;
    return;
  }

  dtmf_sym[dtmf_cnt] = pDtmf->val;
  dtmf_cnt++;

  /* ввели 4 символа */
  if(dtmf_cnt >= 4)
  {
    attempt++;
    if (strcmp (dtmf_sym, pass) == 0)
    {
      *pass_fl = true;
      //say.pass_accept();
    }
    else
    {
      *pass_fl = false;
      //say.wrong_pass();
    }

    dtmf_cnt = 0;
  }

  /* 3 попытки ввода пароля, кладём трубку */
  if(attempt >= 3)
    sim76xx_send_at_com(STOP_CALL, "OK", NULL, 1000);
}

/**
  тест-функция обработки dtmf комбинций
  const sExtraMsg * pDtmf - указатель на полученный dtmf сигнал
  bool flush - флаг очистки
  noreturn
*/
void dtmfLed(const sExtraMsg * pDtmf, bool flush)
{
  static char dtmf_sym[4] = {0};         //Строка dtmf символов
  static bool enter_dtmf_com = true;     //Флаг ввода команды
  static uint8_t dtmf_cnt = 0;           //Счётчик для dtmf сиволов

  if(flush)
  {
    dtmf_cnt = 0;
    enter_dtmf_com = true;
    return;
  }

  dtmf_sym[dtmf_cnt] = pDtmf->val;
  dtmf_cnt++;

  if(dtmf_cnt >= 3)
  {
    //ввели три символа
    if(dtmf_sym[0] == '#')
    {
      switch(dtmf_sym[1])
      {
        case '1':
        if(dtmf_sym[2] == '1')
          sim76xx_send_at_com(VD1_SET, "OK", NULL, 3000);
        else if(dtmf_sym[2] == '0')
          sim76xx_send_at_com(VD1_RESET, "OK", NULL, 3000);
        else enter_dtmf_com = false;
        break;

        case '2':
        if(dtmf_sym[2] == '1')
          sim76xx_send_at_com(VD2_SET, "OK", NULL, 3000);
        else if(dtmf_sym[2] == '0')
          sim76xx_send_at_com(VD2_RESET, "OK", NULL, 3000);
        else enter_dtmf_com = false;
        break;

        case '3':
        if(dtmf_sym[2] == '1')
          sim76xx_send_at_com(VD3_SET, "OK", NULL, 3000);
        else if(dtmf_sym[2] == '0')
          sim76xx_send_at_com(VD3_RESET, "OK", NULL, 3000);
        else enter_dtmf_com = false;
        break;

        case '4':
        if(dtmf_sym[2] == '1')
          sim76xx_send_at_com(VD4_SET, "OK", NULL, 3000);
        else if(dtmf_sym[2] == '0')
          sim76xx_send_at_com(VD4_RESET, "OK", NULL, 3000);
        else enter_dtmf_com = false;
        break;

        default:
        enter_dtmf_com = false;
        break;
      }

    }
    else
      enter_dtmf_com = false;

//    if(enter_dtmf_com)
//      say.dtmf_ok();
//    else
//      say.dtmf_err();

    enter_dtmf_com = true;
    dtmf_cnt = 0;
  }
}