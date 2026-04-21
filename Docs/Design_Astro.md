# Astro Module Design Document

**Location:** `Code/App/Astro/`  
**Files:** `Astro.h`, `Astro.c`

---

## Purpose

The Astro module is a self-contained algorithm module. It calculates local sunrise and sunset times
(expressed as minutes from midnight) for a given calendar date, latitude, longitude, and timezone.
It holds no persistent state beyond the last calculated result and has no dependency on any other
application module — it is a pure computation unit.

The Scheduler calls `Astro_UpdateForDate()` once per day (at midnight or on date change) and then
reads the resulting times via `Astro_GetSunriseMinutes()` / `Astro_GetSunsetMinutes()` every minute
to decide whether an astronomical switching event should occur.

---

## Design Decisions

- **No direct Config dependency.** The Astro module accepts date, latitude, longitude, and timezone
  as function arguments so it can be tested independently of the CfgDataStore module.
- **int16_t for minutes.** Sunrise/sunset results are returned as `int16_t` (minutes from midnight,
  0–1439). This is sufficient for any time of day and avoids floating-point at the API boundary.
- **Single static result.** Calculated times are stored in two module-private `static int16_t`
  variables updated only by `Astro_UpdateForDate()`.

---

## API Reference

### `void Astro_Init(void)`

| | |
|---|---|
| **Purpose** | Initialises the module; resets internal sunrise/sunset values to defaults. |
| **Input** | None |
| **Output / Return** | None |
| **Side effects** | Sets internal sunrise to `360` (06:00) and sunset to `1080` (18:00) as safe placeholder defaults. |
| **Call context** | Called once at system startup before any other Astro function. |

---

### `void Astro_UpdateForDate(int16_t year, uint8_t month, uint8_t day)`

| | |
|---|---|
| **Purpose** | Calculates and stores sunrise and sunset times for the given date. |
| **Input** | `year` — 4-digit calendar year (e.g. 2026) |
| | `month` — month of year, 1–12 |
| | `day` — day of month, 1–31 |
| **Output / Return** | None (results stored internally, retrieved via Get functions) |
| **Side effects** | Updates the module-private sunrise and sunset minute values. |
| **Call context** | Called by the Scheduler once per day, typically at midnight or after a date change is detected. The Scheduler supplies location/timezone values read from CfgDataStore. |

---

### `int16_t Astro_GetSunriseMinutes(void)`

| | |
|---|---|
| **Purpose** | Returns the sunrise time from the most recent `Astro_UpdateForDate()` call. |
| **Input** | None |
| **Output / Return** | `int16_t` — minutes from local midnight (e.g. 360 = 06:00). Range: 0–1439. |
| **Side effects** | None |
| **Call context** | Called by the Scheduler each minute tick to determine if a sunrise switching event applies to the current time. |

---

### `int16_t Astro_GetSunsetMinutes(void)`

| | |
|---|---|
| **Purpose** | Returns the sunset time from the most recent `Astro_UpdateForDate()` call. |
| **Input** | None |
| **Output / Return** | `int16_t` — minutes from local midnight (e.g. 1080 = 18:00). Range: 0–1439. |
| **Side effects** | None |
| **Call context** | Called by the Scheduler each minute tick to determine if a sunset switching event applies to the current time. |

---

## Data Flow

```
CfgDataStore (latitude, longitude, timezone, astro_offset)
        |
        v
Astro_UpdateForDate(year, month, day)  <-- called by Scheduler at midnight
        |
        v
 [internal: g_SunriseMinutes, g_SunsetMinutes]
        |
        +---> Astro_GetSunriseMinutes()  --> Scheduler
        +---> Astro_GetSunsetMinutes()   --> Scheduler
```
