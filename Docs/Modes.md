# Modes Module

Purpose
- Represent and manage device operating modes: Auto, Random, Holiday, Manual.

Public API (from `Code/App/Modes/Modes.h`)
- `typedef enum { MODE_AUTO, MODE_RANDOM, MODE_HOLIDAY, MODE_MANUAL } ModeType;`
- `void Modes_Init(void);`
- `ModeType Modes_GetCurrent(void);`

Behavior & notes
- Modes should be persisted in `Config` and expose a simple API for other modules to query the current mode.
- For Holiday mode, the module should expose the holiday date range.

Implementation TODOs
- Add functions for setting the mode and configuring holiday ranges.
- Add unit tests verifying mode precedence and interaction with Scheduler.

Examples
- `Modes_Init();`