# Scheduler Module

Purpose
- Evaluate programs and astro rules at minute granularity and decide final channel output.

Public API (from `Code/App/Scheduler/Scheduler.h`)
- `void Scheduler_Init(void);`
- `void Scheduler_RunMinuteTick(void);` // should be called every minute

Behavior & notes
- Scheduler merges inputs from:
  - Astro (sunrise/sunset)
  - Programs (daily/yearly)
  - Modes (manual override, random/holiday filters)
  - Manual channel flags
- Priority rules:
  1. Manual override (highest priority)
  2. Holiday mode suppression
  3. Random mode behavior
  4. Programs and Astro events

Implementation TODOs
- Implement the decision chain, apply offsets from Config, and call `Channels_SetState()` accordingly.
- Add unit tests covering priority rules and overlap scenarios.

Examples
- On each minute tick: `Scheduler_RunMinuteTick();`