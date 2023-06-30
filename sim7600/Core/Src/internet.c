#include "internet.h"
#include "sim76xx.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/**
const char * q - ��������� �� ������ get �������
char * dest - ��������� �� �� ���� ������ ���������
size_t sized - ������ ������
return NET_OK/NET_ERR
*/
eNetAnsw sim76xx_https_get_read(const char * q, char * dest, size_t sized)
{
  char char_len[64] = {0};              //����� ���������� ���������
  char send_http_read[64] = {0};        //������������ ��������� http read
  int i = 0;                            //������� ������ ����� ������
  int num = 0;                          //��� ����� ������ � �����
  eComRes res;

  if(sized > ANSWER_SIZE || sized == 0) return NET_ERR_SIZE;
  /* ����� ������ */
  if(sim76xx_send_at_com("AT+HTTPINIT\r\n", "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* ������ */
  if(sim76xx_send_at_com(q, "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* Do */
  if(sim76xx_send_at_com("AT+HTTPACTION=0\r\n", "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* ������� ����������� */
  if(sim76xx_send_at_com(WAIT_STATE, "+HTTPACTION:", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* ������� ���� � ������ ? */
  if(sim76xx_send_at_com("AT+HTTPREAD?\r\n", "+HTTPREAD: LEN", char_len, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
//  /* ����� HTTPREAD ������ ���� ��*/
//  if(sim76xx_send_at_com(WAIT_STATE, "OK", char_len, 1000) != AT_SEND_OK)
//    return NET_ERR_COM;
  /* ���� ������ ������ */
  if(strlen(char_len) == 0)
  {
    if(sim76xx_send_at_com("AT+HTTPTERM\r\n", "OK", NULL, 120000) != AT_SEND_OK)
      return NET_ERR_COM;
    return NET_ERR_COM;
  }
  /* ����� ����� � ������ */
  while(char_len[i++] != '\0')
    if(isdigit(char_len[i])) break;
  /* ������� � ����� */
  num = atoi(&char_len[i]);
  /* ���������� ������ ��� ������� num ���� */
  sprintf(send_http_read, "AT+HTTPREAD=%d\r\n", num);
  /* �������� � ������� */
  if(sim76xx_send_at_com(send_http_read, "+HTTPREAD: DATA", dest, 120000) != AT_SEND_OK)
    return NET_ERR_COM;
  /* ������� ������ */
  if(sim76xx_send_at_com("AT+HTTPTERM\r\n", "OK", NULL, 120000) != AT_SEND_OK)
    return NET_ERR_COM;

  return NET_OK;
}


/**
get ������ update_id �� ���� ��-����
size_t * id - ��������� ���� ������� update_id
const char * const token - ��������� �� ������ �����
return eNetAnsw
*/
eNetAnsw tg_update_id(size_t * id,
                      const char * const token,
                      char * dest,
                      size_t sized)
{
  eNetAnsw res;
  char req_b[120] = {0};    //����� ��� ����������� ������� � �������
  char * pfind = NULL;      //��� �������� � ������
  int i = 0;

  sprintf(req_b,"AT+HTTPPARA=\"URL\",\"https://api.telegram.org/bot%s/getupdates?offset=-1\"\r\n", token);

  if((res = sim76xx_https_get_read(req_b, dest, sized)) != NET_OK)
    return res;

  /* ����� update_id */
  if( (pfind = strstr(dest, "\"update_id\":"))  && (strlen(pfind) != 0) )
  {
    while( (pfind[i] != '\0') && (i < sized - 2) )
    {
      i++;
      /* ����� ����� */
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
get ������ �������� ������ � ��� ��-����
const char * msg - ��������� �� ���������
size_t msg_size - ������ ���������
const char * const token - ������ �����
const char * const chat_id - ��� id ����
*/
eNetAnsw tg_send_msg(const char * msg,
                     size_t msg_size,
                     const char * const token,
                     const char * const chat_id)
{
  char buff[512] = {0};     //����� ��� ��������

  /* �������� ������ */
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