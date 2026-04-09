# RTC Module

Purpose
- Real-time clock interface for retrieving and setting system date/time.

Public API (from `Code/App/RTC/RTC.h`)
- `void RTC_Init(void);`
- `void RTC_GetDateTime(int *year, int *month, int *day, int *hour, int *minute, int *second);`

Behavior & notes
- RTC should persist time across resets using backup battery when available.
- Provide clock correction and DST handling where appropriate.

Implementation TODOs
- Implement RTC read/write using HAL; add tests for edge cases (leap years, DST transitions).

Examples
- `RTC_Init(); RTC_GetDateTime(&y,&m,&d,&h,&min,&s);`