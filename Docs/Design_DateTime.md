# DateTime Module Design Document

**Location:** `Code/App/DateTime/`  
**Files:** `DateTime.h`, `DateTime.c`

---

## Purpose

DateTime is the application-layer interface to the real-time clock. It wraps `HAL_RTC` to isolate
the rest of the application from hardware-specific RTC register access. In the future it will also
apply a software clock-correction offset (configurable via CfgDataStore) to compensate for RTC
crystal drift.

All application code (Scheduler, LUI, ModuleTest) reads and writes the current date/time exclusively
through DateTime, never directly through HAL_RTC.

---

## Design Decisions

- **Decouples app from HAL.** Application modules include `DateTime.h`; only `DateTime.c` includes
  `HAL_RTC.h`. Swapping the RTC peripheral requires changes only in `DateTime.c` and `HAL_RTC.c`.
- **Clock correction hook.** A software-trimming offset (±12.6 s/day, 0.1 s/day resolution) will be
  applied in `DateTime_Get()` once CfgDataStore exposes the correction value. The function signature
  does not change.
- **Split-second parameter types.** `year` is `int16_t` (supports year 0–32767); all other fields
  are `uint8_t` to avoid sign issues.

---

## API Reference

### `void DateTime_Init(void)`

| | |
|---|---|
| **Purpose** | Initialises the DateTime module and the underlying HAL_RTC peripheral. |
| **Input** | None |
| **Output / Return** | None |
| **Side effects** | Calls `HAL_RTC_Init()` to configure the hardware RTC. Enables the backup power path so the RTC continues during main-power loss. |
| **Call context** | Called once at system startup from `main()`, after `HAL_RTC_Init()` has been invoked at HAL level. |

---

### `void DateTime_Get(int16_t* year, uint8_t* month, uint8_t* day, uint8_t* hour, uint8_t* minute, uint8_t* second)`

| | |
|---|---|
| **Purpose** | Reads the current date and time. Any pointer argument that is `NULL` is silently ignored, allowing partial reads. |
| **Input** | All parameters are output pointers |
| **Output** | `*year` — 4-digit year (e.g. 2026) |
| | `*month` — month 1–12 |
| | `*day` — day 1–31 |
| | `*hour` — hour 0–23 |
| | `*minute` — minute 0–59 |
| | `*second` — second 0–59 |
| **Return** | None |
| **Side effects** | None. The function is read-only with respect to hardware state. |
| **Call context** | Called by the Scheduler each minute tick to obtain the current time for program and astronomical event evaluation. Also called by LUI to display the current time. |

---

### `void DateTime_Set(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)`

| | |
|---|---|
| **Purpose** | Sets the RTC to the specified date and time. |
| **Input** | `year` — 4-digit year |
| | `month` — month 1–12 |
| | `day` — day 1–31 |
| | `hour` — hour 0–23 |
| | `minute` — minute 0–59 |
| | `second` — second 0–59 |
| **Output / Return** | None |
| **Side effects** | Calls `HAL_RTC_SetTime()` to write the new time to the hardware RTC registers. |
| **Call context** | Called by LUI when the user performs a manual time-set operation. |

---

## Data Flow

```
HAL_RTC_GetTime()
        |
        v
   DateTime_Get()  <-- Scheduler (every minute), LUI (display)
        |
        v
   [optional: apply clock-correction offset — TODO]
        |
        v
   caller receives year, month, day, hour, minute, second

LUI (user time-set)
        |
        v
   DateTime_Set()
        |
        v
   HAL_RTC_SetTime()
```
