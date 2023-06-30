#ifndef _SMS_H_
#define _SMS_H_

#include <stdint.h>
#include <stdlib.h>

//4SYM(UCS2) * 160(sms size) + \0
#define SMS_BUF_SIZE    (641U)

#define MASTER_N        (const char *)("+79958904709")
//+79958904709


/* Прочитать смс из памяти sim76xx */
int readSms(const char smsn, char * psms_buf, const size_t size);

/* Отправка текстового сообщения на номер */
void sendSms(const char * txt, const char * number);

/* Разбор смс из памяти sim76xx*/
void parsingSms(char * psms, size_t size);

/* */
void smsWork(const char * const psms);

/* UCS2 строку в ASCII */
void UCS2toChar(const char * p_in_str, char * res_str, const size_t size_res);

/* ASCII строку в UCS2 строку */
void ChartoUCS2(const char * p_in_str, char * res_str, const size_t size_res);

/* isstring? Проверялка, на нуль символ в передаваемом массиве.
Нужна, т.к. используются strlen и т.п.*/
int isString(const char * p, size_t size);


#endif