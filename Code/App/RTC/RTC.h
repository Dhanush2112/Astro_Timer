/* RTC.h
 * Real-time clock interface
 */

#ifndef RTC_H
#define RTC_H

#include <stdint.h>

void RTC_Init(void);
void RTC_GetDateTime(int *year, int *month, int *day, int *hour, int *minute, int *second);

#endif /* RTC_H */