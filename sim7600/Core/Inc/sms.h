#ifndef _SMS_H_
#define _SMS_H_

#include <stdint.h>
#include <stdlib.h>

//4SYM(UCS2) * 160(sms size) + \0
#define SMS_BUF_SIZE    (641U)

#define MASTER_N        (const char *)("+79958904709")
//+79958904709


/* ��������� ��� �� ������ sim76xx */
int readSms(const char smsn, char * psms_buf, const size_t size);

/* �������� ���������� ��������� �� ����� */
void sendSms(const char * txt, const char * number);

/* ������ ��� �� ������ sim76xx*/
void parsingSms(char * psms, size_t size);

/* */
void smsWork(const char * const psms);

/* UCS2 ������ � ASCII */
void UCS2toChar(const char * p_in_str, char * res_str, const size_t size_res);

/* ASCII ������ � UCS2 ������ */
void ChartoUCS2(const char * p_in_str, char * res_str, const size_t size_res);

/* isstring? ����������, �� ���� ������ � ������������ �������.
�����, �.�. ������������ strlen � �.�.*/
int isString(const char * p, size_t size);


#endif