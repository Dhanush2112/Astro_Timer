# Astro_Timer
Developing an astronomical timer on Renesas RA4C1 series Cortex-M33 microcontroller.

## Project layout

Code/
  App/
    Astro/        - Astronomical calculations and offsets (Astro.h/.c)
    Channels/     - Physical channel control (Channels.h/.c)
    Programs/     - Time programs management (Programs.h/.c)
    Config/       - Persistent configuration (Config.h/.c)
    Data/         - Data storage and helpers (Data.h/.c)
    Menu/         - User interface menu (Menu.h/.c)
    Modes/        - Operating modes (Modes.h/.c)
    RTC/          - Real-time clock (RTC.h/.c)
    Scheduler/    - Scheduling and program execution (Scheduler.h/.c)
  HAL/            - Hardware Abstraction Layer
  ModuleTest/     - Module-level tests and harnesses

Docs/
  Design_Reference.md - Functional requirements and design notes

Notes
- The project uses canonical module filenames (e.g., `Astro.h`/`Astro.c`).
- New placeholder modules created: `Channels`, `Programs`, `Config`. These contain minimal implementations to be expanded.

Next steps
- Implement the Astro calculations (sunrise/sunset) using location and timezone from `Config`.
- Implement full Programs support (daily/yearly rules) and Scheduler integration.
- Add unit tests under `ModuleTest/` and run a CI build to validate the changes.