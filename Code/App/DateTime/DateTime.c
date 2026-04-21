/* DateTime.c
 * Date and time implementation.
 * Delegates to HAL_RTC for hardware access; applies configured clock correction.
 */

#include "DateTime.h"

void DateTime_Init(void)
{
  /* TODO: initialise HAL_RTC */
}

void DateTime_Get(int16_t *year,
                  uint8_t *month,
                  uint8_t *day,
                  uint8_t *hour,
                  uint8_t *minute,
                  uint8_t *second)
{
  /* TODO: call HAL_RTC_GetTime() and apply clock-correction offset */
  if (year)
    *year = 2026;
  if (month)
    *month = 1;
  if (day)
    *day = 1;
  if (hour)
    *hour = 0;
  if (minute)
    *minute = 0;
  if (second)
    *second = 0;
}

void DateTime_Set(int16_t year,
                  uint8_t month,
                  uint8_t day,
                  uint8_t hour,
                  uint8_t minute,
                  uint8_t second)
{
  /* TODO: call HAL_RTC_SetTime() */
  (void)year;
  (void)month;
  (void)day;
  (void)hour;
  (void)minute;
  (void)second;
}
