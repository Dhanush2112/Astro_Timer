# Astronomical Timer – Features and Data Reference

## Core Function
- Dual-channel astronomical time-based switching device
- Automatic control of electrical loads based on calculated sunrise and sunset times
- Fully autonomous operation without external optical sensors

## Astronomical Calculation
- Sunrise and sunset times are calculated daily
- Calculations depend on:
  - Current date
  - Latitude
  - Longitude
  - Time zone
- Astronomical times are automatically adjusted throughout the year

## Astronomical Offset
- Global offset applied to sunrise and sunset times
- Offset range: ±120 minutes
- Offset value is applied equally to both channels
- Offset remains fixed until updated by the user

## Channel Switching Modes
Each channel independently supports:
- Sunset → ON, Sunrise → OFF
- Sunrise → ON, Sunset → OFF
- Astronomical switching disabled

## Time Programs
- Maximum number of programs: 100 (shared across both channels)
- Time resolution: 1 minute
- Supported program types:
  - Daily programs (weekday-based)
  - Yearly programs (specific calendar date)

## Combined Operation
- Astronomical switching and time programs can operate at the same time on a single channel
- Multiple switching conditions may apply to a channel simultaneously

## Operating Modes
- Automatic mode (program-controlled switching)
- Random switching mode
- Holiday (block) mode
- Manual override mode

## Random Mode
- Random switching interval range: 10–120 minutes
- Random mode replaces scheduled switching while active

## Holiday Mode
- User-defined start and end date
- Automatic switching suppressed during the holiday period

## Manual Mode
- Manual ON/OFF control per channel
- Manual state overrides all automatic switching

## Real-Time Clock
- Internal real-time clock maintains time and date
- RTC continues operation during power loss using backup battery
- Backup retention duration: up to 3 years
- Accuracy target: ±1 second per day at 23 °C

## Time Correction
- User-adjustable clock correction
- Adjustment range: ±12.6 seconds per day
- Adjustment resolution: 0.1 seconds per day

## Daylight Saving Time
- Automatic summer/winter time switching
- Switching behavior based on selected geographic region

## Location Configuration
- Location can be defined by:
  - Predefined country and city selection
  - Manual entry of latitude, longitude, and time zone
- Manual entry requires all parameters to be provided together

## Data Retention
- All user settings retained during power loss
- Normal operation resumes automatically when power is restored

## Display Indicators
- Individual channel status indication
- Mode indicators for:
  - Automatic operation
  - Offset active
  - Random mode
  - Holiday mode
  - Manual mode

## Physical Channels
- Two independent output channels
- Each channel supports independent configuration and operation

---

## Module API Summary (high-level)
This section maps the Design Reference concepts to the project's canonical modules and lists the key public functions and types to be implemented in source headers found under `Code/App/*`.

- Astro (Code/App/Astro)
  - Purpose: Calculate daily astronomical times (sunrise/sunset) from date, latitude, longitude, and timezone; apply global offset.
  - Key functions (in `Astro.h`):
    - `void Astro_Init(void);`
    - `void Astro_UpdateForDate(int year, int month, int day);`
    - `int Astro_GetSunriseMinutes(void);`  // minutes from local midnight
    - `int Astro_GetSunsetMinutes(void);`   // minutes from local midnight

- Channels (Code/App/Channels)
  - Purpose: Control and query the two physical output channels.
  - Key functions (in `Channels.h`):
    - `void Channels_Init(void);`
    - `void Channels_SetState(uint8_t channel, bool on);`
    - `bool Channels_GetState(uint8_t channel);`
  - Notes: `CHANNEL_COUNT` is defined (currently 2). Hardware-level mapping should live in HAL.

- Programs (Code/App/Programs)
  - Purpose: Store and manage scheduled time programs (daily and yearly) up to the maximum allowed.
  - Key functions (in `Programs.h`):
    - `void Programs_Init(void);`
    - `int Programs_Add(ProgramType type, const char *spec);` // returns id or -1
    - `void Programs_Remove(int id);`
  - Types:
    - `typedef enum { PROGRAM_DAILY, PROGRAM_YEARLY } ProgramType;`
  - Notes: Program parsing and evaluation (matching current date/time) should be implemented by the Programs/Scheduler modules.

- Config (Code/App/Config)
  - Purpose: Persistent configuration storage (location, timezone, offsets, user preferences).
  - Key API (in `Config.h`):
    - `typedef struct { double latitude; double longitude; int timezone_offset_minutes; int astro_offset_minutes; } SystemConfig;`
    - `void Config_Load(SystemConfig *cfg);`
    - `void Config_Save(const SystemConfig *cfg);`
  - Notes: Integrate with platform-specific persistent storage (HAL) for read/write.

- Data (Code/App/Data)
  - Purpose: Generic data helpers and storage helpers used by Programs and Config modules.
  - Key functions (in `Data.h`):
    - `void Data_Init(void);`

- Menu (Code/App/Menu)
  - Purpose: User interface layer for configuring programs, mode selection, offsets, and manual control.
  - Key functions (in `Menu.h`):
    - `void Menu_Init(void);`

- Modes (Code/App/Modes)
  - Purpose: Track and expose the current operating mode (Auto, Random, Holiday, Manual).
  - Key functions/types (in `Modes.h`):
    - `typedef enum { MODE_AUTO, MODE_RANDOM, MODE_HOLIDAY, MODE_MANUAL } ModeType;`
    - `void Modes_Init(void);`
    - `ModeType Modes_GetCurrent(void);`

- RTC (Code/App/RTC)
  - Purpose: Real-time clock interface for retrieving and setting date/time.
  - Key functions (in `RTC.h`):
    - `void RTC_Init(void);`
    - `void RTC_GetDateTime(int *year, int *month, int *day, int *hour, int *minute, int *second);`
  - Notes: Implement clock correction and backup retention per hardware capabilities.

- Scheduler (Code/App/Scheduler)
  - Purpose: Evaluate Programs and Astro rules each minute and invoke channel state changes.
  - Key functions (in `Scheduler.h`):
    - `void Scheduler_Init(void);`
    - `void Scheduler_RunMinuteTick(void);` // called every minute to evaluate rules
  - Notes: Scheduler merges multiple inputs (astro times, programs, manual override, mode filters) and decides final channel output.

### Integration notes
- Astro calculations are required daily; Scheduler should call `Astro_UpdateForDate(...)` once at midnight (or after date change) and then query `Astro_GetSunriseMinutes()`/`Astro_GetSunsetMinutes()`.
- `Config` provides location/timezone and the global astro offset which must be applied when the Scheduler evaluates astronomical events.
- Manual mode and manual overrides must take precedence over automatic scheduler decisions; Scheduler should check `Modes_GetCurrent()` and any persistent manual flags before changing channels.
- Random mode and holiday mode act as filters on scheduled events and should be implemented in the Scheduler decision chain.

---

(End of Design Reference additions)
