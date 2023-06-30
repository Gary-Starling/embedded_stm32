#include "internet.h"
#include "sim76xx.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/**
const char * q - указатель на строку get запроса
char * dest - указатель на то куда класть результат
size_t sized - размер буфера
return NET_OK/NET_ERR
*/
eNetAnsw sim76xx_https_get_read(const char * q, char * dest, size_t sized)
{
  char char_len[64] = {0};              //длина принимаего сообщения
  char send_http_read[64] = {0};        //составляемое сообщение http read
  int i = 0;                            //счётчик поиска конца строки
  int num = 0;                          //для каста строки в число
  eComRes res;

  if(sized > ANSWER_SIZE || sized == 0) return NET_ERR_SIZE;
  /* Старт сессии */
  if(sim76xx_send_at_com("AT+HTTPINIT\r\n", "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* Запрос */
  if(sim76xx_send_at_com(q, "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* Do */
  if(sim76xx_send_at_com("AT+HTTPACTION=0\r\n", "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* Ожидаие выполнениея */
  if(sim76xx_send_at_com(WAIT_STATE, "+HTTPACTION:", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* Сколько байт в ответе ? */
  if(sim76xx_send_at_com("AT+HTTPREAD?\r\n", "+HTTPREAD: LEN", char_len, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
//  /* После HTTPREAD должен быть ОК*/
//  if(sim76xx_send_at_com(WAIT_STATE, "OK", char_len, 1000) != AT_SEND_OK)
//    return NET_ERR_COM;
  /* Если нечего читать */
  if(strlen(char_len) == 0)
  {
    if(sim76xx_send_at_com("AT+HTTPTERM\r\n", "OK", NULL, 120000) != AT_SEND_OK)
      return NET_ERR_COM;
    return NET_ERR_COM;
  }
  /* Найдём цифры в строке */
  while(char_len[i++] != '\0')
    if(isdigit(char_len[i])) break;
  /* Кастанём в число */
  num = atoi(&char_len[i]);
  /* Подготовим строку для запроса num байт */
  sprintf(send_http_read, "AT+HTTPREAD=%d\r\n", num);
  /* Отправим и считаем */
  if(sim76xx_send_at_com(send_http_read, "+HTTPREAD: DATA", dest, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* Закроем сессию */
  if(sim76xx_send_at_com("AT+HTTPTERM\r\n", "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;

  return NET_OK;
}


/**
get запрос update_id из чата тг-бота
size_t * id - указатель куда запишем update_id
const char * const token - указатель на строку токен
return eNetAnsw
*/
eNetAnsw tg_update_id(size_t * id,
                      const char * const token,
                      char * dest,
                      size_t sized)
{
  eNetAnsw res;
  char req_b[120] = {0};    //буфер для составления запроса с токеном
  char * pfind = NULL;      //для парсинга и поиска
  int i = 0;

  sprintf(req_b,"AT+HTTPPARA=\"URL\",\"https://api.telegram.org/bot%s/getupdates?offset=-1\"\r\n", token);

  if((res = sim76xx_https_get_read(req_b, dest, sized)) != NET_OK)
    return res;

  /* поиск update_id */
  if( (pfind = strstr(dest, "\"update_id\":"))  && (strlen(pfind) != 0) )
  {
    while( (pfind[i] != '\0') && (i < sized - 2) )
    {
      i++;
      /* нашли цифру */
      if(isdigit(pfind[i]))
      {
        *id = atoi(&pfind[i]);
        return NET_OK;
      }
    }
    return ERR_NOT_NET;
  }
  else
    return ERR_NOT_NET; //
}

/**
get запрос отправки текста в чат тг-бота
const char * msg - указатель на сообщение
size_t msg_size - размер сообщения
const char * const token - строка токен
const char * const chat_id - чат id бота
*/
eNetAnsw tg_send_msg(const char * msg,
                     size_t msg_size,
                     const char * const token,
                     const char * const chat_id)
{
  char buff[512] = {0};     //буфер для отправки

  /* Составим запрос */
  sprintf(buff,
          "AT+HTTPPARA=\"URL\",\"https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s\"\r\n",
          token, chat_id, msg);

  if(sim76xx_send_at_com("AT+HTTPINIT\r\n", "OK", NULL, 120000)!= AT_SEND_OK)
    return NET_ERR_COM;
  if(sim76xx_send_at_com(buff, "OK", NULL, 120000)!= AT_SEND_OK)
    return NET_ERR_COM;
  if(sim76xx_send_at_com("AT+HTTPACTION=0\r\n", "OK", NULL, 120000)!= AT_SEND_OK)
    return NET_ERR_COM;
  if(sim76xx_send_at_com(WAIT_STATE, "+HTTPACTION:", NULL, 120000)!= AT_SEND_OK)
    return NET_ERR_COM;
  if(sim76xx_send_at_com("AT+HTTPTERM\r\n", "OK", NULL, 120000)!= AT_SEND_OK)
    return NET_ERR_COM;

  return NET_OK;
}