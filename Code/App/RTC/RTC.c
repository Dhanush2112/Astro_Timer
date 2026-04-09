/* RTC.c
 * RTC implementation (placeholder)
 */

#include "RTC.h"

void RTC_Init(void) {
    // placeholder
}

void RTC_GetDateTime(int *year, int *month, int *day, int *hour, int *minute, int *second) {
    if (year) *year = 2026;
    if (month) *month = 1;
    if (day) *day = 1;
    if (hour) *hour = 0;
    if (minute) *minute = 0;
    if (second) *second = 0;
}