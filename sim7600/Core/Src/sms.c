#include "sms.h"
#include <string.h>
#include <stdio.h>

#include "sim76xx.h"


/**
  ��������� ��� �� ������ sim76xx
  const char smsn - ����� ��� � ������.
  char * const psms_buf - �������� �� �����, ���� �������� ���
  const ssize_t size - ������ ������
  return int - �������� ����������
*/
int readSms(const char smsn, char * psms_buf, const size_t size)
{
  char comm[15] = {0};

  /* ��� ����� ��� 160 * 4(UCS2) + \0  */
  if(size < SMS_BUF_SIZE)
    return 1;

  /* ��������� ������� */
  sprintf(comm, "AT+CMGRD=%u\r\n", smsn);
  return sim76xx_send_at_com(comm, "+CMGRD:", psms_buf, 3000);
}



/**
  ������� ��� �� ������ sim76xx.
  ������� �������� ����� "���������"
  char * psms - ��������� �� ����� � ���
  noreturn
*/

/* example
0 - "+CMGRD: "; 1 - " " 2 - "REC UNREAD"; 3 - "NUMBER"; 4 - "date";
5 - "time"; 6 - "msg"; 7 - "OK"; 8 - "\n"; */
void parsingSms(char * psms, size_t size)
{
  uint8_t cnt = 0;

  char numb[14];                //��� ������������ ������ �������� � ASCII
  char msg[161];                //��� �������� ���������
  char * ptr = NULL;            //��� strtok

  /* ������ ������� */
  if(psms == NULL)
    return;

  if(!isString(psms, size)) return;

  ptr = strtok(psms, "\r\n,\"");

  while (ptr != NULL)
  {
    switch(cnt)
    {
      default: break;
      /* Number */
      case 2:
      UCS2toChar(ptr,  numb, sizeof(numb));             //���������� �������� �����
      if(strstr((const char *)numb, MASTER_N) == NULL)  //������ �����?
        return;
      break;
      /* MSg */
      case 5:
      UCS2toChar(ptr, msg, sizeof(msg));
      smsWork(msg); //handle
      break;
      /* Data / time / etc */
    }
    cnt++;
    ptr = strtok(NULL, "\r\n,\"");
  }

  sendSms("���������", numb);
}

/**
const char * txt - ASCII ������ sms ���������
const char * number - ASCII �����, �� ������� ��� ���������
*/
void sendSms(const char * txt, const char * number)
{
  char ucs2_numb[53] = {0};      //13sym * 4 + \0
  char ucs2_msg[81];             //��������� �� �������� � UCS2(� ��� �� ����� 20 �������� (20 * 4 + \0 )

  sim76xx_send_at_com("AT+CSMP=17,167,0,8\r\n", "OK", NULL, 1000);

  /* ���������� ����� �� ascii � ucs2 */
  ChartoUCS2(number, ucs2_numb, sizeof(ucs2_numb));

  /* ���������� ������� �� �������� ���������, � ������� */
  sprintf(ucs2_msg, "AT+CMGS=\"%s\"\r\n", ucs2_numb);

  /* �������� � � ������� ����������� */
  sim76xx_send_at_com(ucs2_msg, "> ", NULL, 1000);

  /* ���������� ��� */
  memset(ucs2_msg, 0x00, sizeof(ucs2_msg));
  ChartoUCS2(txt, ucs2_msg, sizeof(ucs2_msg));

  /* ������� ����� ����� */
  ucs2_msg[strlen(ucs2_msg)] = 0x1A;

  /* �������� � ����� ����� \r\n */
  sim76xx_send_at_com(ucs2_msg, "\r\n", NULL, 1000);

  /* ����� ��� �� */
  sim76xx_send_at_com(WAIT_STATE, "OK", NULL, 1000);
}




/* ���� �������-������� �� ������� ���
min command == 3 sym
*11*20 ok
*10*2 err
*/
void smsWork(const char * const psms)
{
  size_t len;

  if(psms == NULL)
    return;

  len = strlen(psms);

  if(len < 3 && len%3 != 0)
    return;

  for(size_t i = 0; i < len; i+=3)
  {
    if(psms[i] == '*')
    {
      switch(psms[i+1])
      {
        case '1':
        if(psms[i+2] == '1')
          sim76xx_send_at_com(VD1_SET, "OK", NULL, 1000);
        else if(psms[i+2] == '0')
          sim76xx_send_at_com(VD1_RESET, "OK", NULL, 1000);
        break;

        case '2':
        if(psms[i+2] == '1')
          sim76xx_send_at_com(VD2_SET, "OK", NULL, 1000);
        else if(psms[i+2] == '0')
          sim76xx_send_at_com(VD2_RESET, "OK", NULL, 1000);
        break;

        case '3':
        if(psms[i+2] == '1')
          sim76xx_send_at_com(VD3_SET, "OK", NULL, 1000);
        else if(psms[i+2] == '0')
          sim76xx_send_at_com(VD3_RESET, "OK", NULL, 1000);
        break;

        case '4':
        if(psms[i+2] == '1')
          sim76xx_send_at_com(VD4_SET, "OK", NULL, 1000);
        else if(psms[i+2] == '0')
          sim76xx_send_at_com(VD4_RESET, "OK", NULL, 1000);
        break;

        default: break;
      }
    }
  }
}


/**
  UCS2->char
  const char * p_in_str - ������ � ������� UCS2
  char * res_str - ��������� �� �����, ��� ����������� ������ ASCII
  size_res - ������ � ������ res_str
  noreturn
ex:
  ������� ������ ���� const char * "003700300039\0"
  �� ������ char * "Abc 123\0"
*/
void UCS2toChar(const char * p_in_str, char * res_str, uint32_t size_res)
{
  char UcodeSTR[5] = {'\0'};
  uint16_t Ucode = 0x0000;
  uint8_t j = 0;
  size_t len;

   if(p_in_str == NULL || res_str == NULL) return;

  len = strlen(p_in_str);

  /* �������� ������ ������ ���� ������ 4 + 1 '\0' */
  if( len == 0 || len % 4 != 0 ) return;
  if( ((len / 4) + 1) > size_res) return;

  for (size_t i = 0; i < len - 3; i += 4 )
  {
    memcpy(UcodeSTR, &p_in_str[i], 4);              // p_in_str[4] = '\0'
    Ucode = (uint16_t)strtol(UcodeSTR, NULL, 16);   // ��������� � hex

    if (Ucode <= 0x7F)                               //general + en
      res_str[j] = (char)Ucode;
    else if ((Ucode >= 0x410) && (Ucode <= 0x44F))  //ru
      res_str[j] = (char)(Ucode - 0x350);
    else                                            //�������� ������� ��������� � ������
      res_str[j] = 0x20;

    j+= 1;
  }

  res_str[j] = '\0';
}

/**
    char->UCS2
    const char * p_in_str - ������ � ������� ASCII (C-style!)
    char * res_str - ��������� �� �����, ��� ����������� ������ UCS2
    size_res - ������ � ������ res_str
    noreturn
    ex:
    ������� ������ ���� const char * "Abc 123\0"
    �� ������ char *  "003700300039\0"
*/
void ChartoUCS2(const char * p_in_str, char * res_str, const size_t size_res)
{
  uint16_t Ucode = 0x0000;
  size_t step = 0;
  size_t len_in;

  if(p_in_str == NULL || res_str == NULL) return;

  len_in = strlen(p_in_str);

  if( len_in == 0 || (size_res % 4) !=  1) return;
  if( ((len_in * 4) + 1) > size_res) return;

  for (size_t i = 0; i < len_in; i += 1 )
  {
    if(p_in_str[i] <= 0x7F)
      Ucode = p_in_str[i];
    else if (p_in_str[i] >= 0xC0)
      Ucode = p_in_str[i] + 0x350;
    else
      Ucode = 0x0020;

    sprintf(&res_str[step],"%04X", Ucode);

    step+= 4;
  }
  //res_str = '\0';
  //sprintf ������ \0
}


/**
*/
int isString(const char * p, size_t size)
{
  size_t i = 0;

  if(size == 0) return -1;

  while(i != (size - 1))
  {
    if(p[i] == '\0') return 1;
    i++;
  }

  return -1;
}