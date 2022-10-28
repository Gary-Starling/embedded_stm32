#include <kty_81_110.h>
#include <math.h>
#include <median.h>

#define SIZE 10

/* Таблица температуры  InTab - adc/ OutTab - temp */
signed int InTab[] = {849, 916, 985, 1056, 1126, 1199, 1271, 1343, 1415, 1486, 1559, 1628, 1696, 1764, 1830, 1895, 1959, 2020, 2075, 2125, 2166};
signed int OutTab[] = {-50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150};

/**
 * @brief Получение температуры с датчика
 *
 * @param temp - указатель на переменную температуры
 * @param chanel - Номер канала
 * @param corr - коррекция показаний
 * @param adc - указатель на volatile значение АЦП
 * @return Dt_states - состояние датчика
 */
Dt_states get_temp(int *temp, uint8_t chanel, int corr, volatile uint16_t *adc)
{
    unsigned short cnt;   //Счётчик шаг табличный
    uint32_t buf_adc = 0; //Промежуточная перменная для суммы
    float tmp = 0.0;

    if (chanel >= 2)
        return ERROR_T;

    buf_adc = 0;

    /* 10 замеров, делим на 10, немного усредняем */
    for (int j = 0; j < 100; ++j)
        buf_adc += *adc;

    buf_adc = buf_adc / 100;

    /* Возможные ошибки */
    if (buf_adc > 2160)
        return ERROR_T;

    if (buf_adc < 860)
        return ERROR_T;

    cnt = 0;

    while (InTab[++cnt] < buf_adc)
        ;

    if (cnt)
        --cnt;

    tmp = ((float)((float)OutTab[cnt] + ((float)OutTab[cnt + 1] - (float)OutTab[cnt]) * ((float)buf_adc - (float)InTab[cnt]) /
                                            ((float)InTab[cnt + 1] - (float)InTab[cnt])));

    tmp = floor(tmp); //Округлим до меньшего

    if (chanel)
        *temp = MedianFilter1((int)tmp + corr);
    else
        *temp = MedianFilter2((int)tmp + corr);

    // HAL_IWDG_Refresh(&hiwdg);

    if (*temp >= 139)
        return HI_T;

    if (*temp <= -39)
        return LOW_T;

    return OK_T;
}

/**
 * Функция получает
 *   "Dt_states *dt"
 *   "Dt_cnts *dt_n"
 *  и
 *
 *
 *

 */

/**
 * @brief - Ничего не возращает
 * В случае если какой либо из счётчиков будет выше MAX_ERRORS
 * функция выставить в состоянии датчика код ошибки
 *
 * @param dt  - указатель на состояние датчика
 * @param dt_n - указатель на счётчик ошибок
 * @param res - результат замера функцией OK/ERR/HI/LOW = get_temp()
 * @note
 */
void scan_error(Dt_states *dt, Dt_cnts *dt_n, int res)
{

    switch (res)
    {
    case ERROR_T:
        dt_n->con++;
        if (dt_n->con >= MAX_ERRORS)
        {
            dt_n->con = MAX_ERRORS;
            *dt = ERROR_T;
            dt_n->ok = 0;
        }
        break;

    case HI_T:
        dt_n->hi++;
        if (dt_n->hi >= MAX_ERRORS)
        {
            dt_n->hi = MAX_ERRORS;
            *dt = HI_T;
            dt_n->ok = 0;
        }
        break;

    case LOW_T:
        dt_n->low++;
        if (dt_n->low >= MAX_ERRORS)
        {
            dt_n->low = MAX_ERRORS;
            *dt = LOW_T;
            dt_n->ok = 0;
        }
        break;

    case OK_T:
        dt_n->ok++;
        if (dt_n->ok >= 500)
        {
            dt_n->con = 0;
            dt_n->hi = 0;
            dt_n->low = 0;
            *dt = OK_T;
        }
        break;

    default:
        break;
    }
}
