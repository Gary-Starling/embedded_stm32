#ifndef H_KTY_81_110
#define H_KTY_81_110

#include <stdint.h>


/* коды ошибок */
typedef enum  {
  OK_T = 0,
  ERROR_T,
  HI_T,
  LOW_T,
} Dt_states;

typedef struct sensor_errror_counts {
  uint16_t hi;   //Высокая температуры
  uint16_t low;  //Низкая
  uint16_t con;  //Обрыв или замыкание
  uint16_t ok;   //для возврата
} Dt_cnts;


#define DT_1            1
#define DT_2            0

#define MAX_ERRORS      1000


Dt_states get_temp(int *temp, uint8_t chanel, int corr, volatile uint16_t * adc);
void scan_error(Dt_states *dt, Dt_cnts * dt_n, int res);

#endif