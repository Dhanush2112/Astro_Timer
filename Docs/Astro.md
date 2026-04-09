# Astro Module

Purpose
- Calculate daily astronomical events (sunrise, sunset) for a given date and location.

Public API (from `Code/App/Astro/Astro.h`)
- `void Astro_Init(void);`
- `void Astro_UpdateForDate(int year, int month, int day);` — re-calculate sunrise/sunset for the given date
- `int Astro_GetSunriseMinutes(void);` — minutes since local midnight
- `int Astro_GetSunsetMinutes(void);` — minutes since local midnight

Behavior & notes
- Use latitude, longitude and timezone from `Config` to compute times.
- Apply `astro_offset_minutes` from `Config` when returning values used by Scheduler.
- The Scheduler should call `Astro_UpdateForDate(...)` once per date change (midnight or immediately after boot).

Implementation TODOs
- Implement a reliable astronomical algorithm (e.g., NOAA algorithm) with adequate accuracy.
- Add unit tests that validate sunrise/sunset against known test vectors.

Examples
- Call `Astro_Init()` at system startup.
- On date change: `Astro_UpdateForDate(2026, 4, 10);`
- Query times: `int sr = Astro_GetSunriseMinutes(); int ss = Astro_GetSunsetMinutes();`