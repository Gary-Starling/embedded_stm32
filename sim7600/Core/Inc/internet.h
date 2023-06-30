#ifndef _INTERNET_H_
#define _INTERNET_H_

#include <stdint.h>
#include <stdlib.h>

#define TOKEN       (const char * const)("")
#define CHAT_ID     (const char * const)("")

/* Возможные результаты операций */
typedef enum {
  ERR_NOT_NET = -3,
  NET_ERR_SIZE = -2,
  NET_ERR_COM = -1,
  NET_OK  = 0
}eNetAnsw;

/* sim http function */
eNetAnsw sim76xx_https_get_read(const char * q, char * dest, size_t sized);

/* telegram api function */
eNetAnsw tg_update_id(size_t * id,
                      const char * const token,
                      char * dest,
                      size_t sized);

eNetAnsw tg_send_msg(const char * msg,
                     size_t msg_size,
                     const char * const token,
                     const char * const chat_id);

#endif