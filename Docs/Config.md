# Config Module

Purpose
- Persistent storage and retrieval of system configuration (location, timezone, offsets, user prefs).

Public API (from `Code/App/Config/Config.h`)
- `typedef struct { double latitude; double longitude; int timezone_offset_minutes; int astro_offset_minutes; } SystemConfig;`
- `void Config_Load(SystemConfig *cfg);`
- `void Config_Save(const SystemConfig *cfg);`

Behavior & notes
- `timezone_offset_minutes` is the offset from UTC in minutes.
- `astro_offset_minutes` is the global sunrise/sunset offset (±120 minutes) applied to computed times.
- Platform-specific persistent storage (flash, EEPROM) should be accessed via HAL.

Implementation TODOs
- Implement read/write to persistent storage and define defaults.
- Add validation (latitude range, longitude range, timezone bounds, offset limits).

Examples
- Load at boot: `Config_Load(&cfg);`
- Update and save: `cfg.astro_offset_minutes = 15; Config_Save(&cfg);`