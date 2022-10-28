#ifndef MEDIAN_H
#define MEDIAN_H

float MedianFilter1(int datum);
float MedianFilter2(int datum);


/*Фильтр Филом Экстрома*/

#define STOPPER   (-40) /* Smaller than any datum */
#define MEDIAN_FILTER_SIZE (51)

#endif
