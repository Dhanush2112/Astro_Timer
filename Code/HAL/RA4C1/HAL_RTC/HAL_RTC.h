/* HAL_RTC.h
 * Hardware abstraction for the real-time clock peripheral.
 */

#ifndef HAL_RTC_H
#define HAL_RTC_H

#include "common.h"

void HAL_RTC_Init(void);
void HAL_RTC_GetTime(int16_t *year,
                     uint8_t *month,
                     uint8_t *day,
                     uint8_t *hour,
                     uint8_t *minute,
                     uint8_t *second);
void HAL_RTC_SetTime(int16_t year,
                     uint8_t month,
                     uint8_t day,
                     uint8_t hour,
                     uint8_t minute,
                     uint8_t second);

#endif /* HAL_RTC_H */
