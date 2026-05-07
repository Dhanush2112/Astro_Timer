/* DateTime.h
 * Date and time access API (wraps HAL_RTC with clock-correction support)
 */

#ifndef DATETIME_H
#define DATETIME_H

#include "common.h"

void DateTime_Init(void);
void DateTime_Get(int16_t *year,
                  uint8_t *month,
                  uint8_t *day,
                  uint8_t *hour,
                  uint8_t *minute,
                  uint8_t *second);
void DateTime_Set(int16_t year,
                  uint8_t month,
                  uint8_t day,
                  uint8_t hour,
                  uint8_t minute,
                  uint8_t second);

#endif /* DATETIME_H */
