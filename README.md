# Astro Timer

An astronomical timer firmware for the **Renesas RA4C1** microcontroller (Arm Cortex-M33).  
The device calculates local sunrise/sunset times from a configurable geographic location and controls output channels accordingly.

---

## Hardware target

| Item | Detail |
|---|---|
| MCU | Renesas RA4C1 (Arm Cortex-M33, 100 MHz) |
| FPU | FPv5-SP (hardware single-precision only) |
| Flash | 256 KB at `0x00000000` |
| SRAM | 64 KB at `0x20000000` |
| Compiler | IAR Embedded Workbench for ARM 9.2 |

---

## Repository layout

```
Code/
  common.h                   Central stdlib include hub (stdint, stdbool, string, math, float32_t typedef)
  App/
    Scheduler.c/.h           Cooperative task scheduler
    Astro/                   Sunrise/sunset calculations (NOAA algorithm)
    CfgDataStore/            Parameter-block engine — typed get/set, write validation, version-based change detection
      ParameterBlock.c/.h    Public API and RAM mirror
      ParameterBlockTypes.h  All shared types (WriteError_t, LocationBlock_t, Block_t, AstroTmrParameters_t, …)
      WriteChecks.c/.h       Per-parameter range and validity checks
      Private.h              Internal write-check dispatcher
    DateTime/                Date/time helpers and conversions
    LUI/                     Local User Interface (display + input handling)
  HAL/
    RA4C1/
      main.c                 Entry point and system initialisation
      toolchain_iar.cmake    IAR ARM CMake toolchain file
      RA4C1.icf              IAR linker config (flash/SRAM/stack/heap regions)
      HAL_Display/           Display driver abstraction
      HAL_GPIO/              GPIO driver abstraction
      HAL_NVM/               Non-volatile memory driver abstraction
      HAL_RTC/               RTC driver abstraction
  ModuleTest/
    functest_main.c          Shared Unity test entry point (main → UnityMain → RunAllTests)
    Test_Astro.c             Module tests for Astro
    Test_DateTime.c          Module tests for DateTime
    Test_Scheduler.c         Module tests for Scheduler
    Test_CfgDataStore.c      Module tests for CfgDataStore
Docs/
  CodingStandard.md          Project-wide coding standard and naming conventions
  Design_Reference.md        Functional requirements
  Design_Astro.md            Astro module design
  Design_CfgDataStore.md     CfgDataStore module design
  Design_DateTime.md         DateTime module design
  Design_HAL.md              HAL design
  Design_LUI.md              LUI module design
  Design_Scheduler.md        Scheduler module design
  Design_ModuleTest.md       Module test design
```

---

## Build system

CMake 3.25+ with Ninja and three presets defined in `CMakePresets.json`:

| Preset | Toolchain | Output | Build dir |
|---|---|---|---|
| `debug` | IAR ARM | `Astro_01.00.elf/.hex/.bin` (no optimisation, full debug) | `build/debug/` |
| `release` | IAR ARM | `Astro_01.00.elf/.hex/.bin` (size-optimised `-Ohz`) | `build/release/` |
| `test` | Host GCC (MinGW) | 4 test executables | `build/test/` |

### Prerequisites

- CMake ≥ 3.25
- Ninja
- IAR Embedded Workbench for ARM 9.2 (firmware builds)  
  Default install path: `C:/Program Files/IAR Systems/Embedded Workbench 9.2/arm`  
  Override with the environment variable `IAR_ARM_TOOLCHAIN_DIR`.
- MinGW GCC ≥ 13 at `C:/FW_Build_Tools/Mingw32/bin/gcc.exe` (test build)
- Internet access on first test configure (Unity v2.6.1 fetched via `FetchContent`)

### Build commands

```powershell
# Configure + build firmware (debug)
cmake --preset debug
cmake --build --preset debug

# Configure + build firmware (release)
cmake --preset release
cmake --build --preset release

# Configure + build + run module tests
cmake --preset test
cmake --build --preset test
ctest --preset test
```

---

## Module tests

Tests use the **Unity** framework (v2.6.1, fixture extension enabled).  
Each module has a dedicated test executable; `functest_main.c` provides the shared `main()`.

```
test_astro          — Astro sunrise/sunset calculation tests
test_datetime       — DateTime helper tests
test_scheduler      — Scheduler task dispatch tests
test_cfgdatastore   — CfgDataStore get/set/validate/update tests
```

All four tests must pass before merging changes.

---

## Key design decisions

- **`float32_t` for coordinates** — `typedef float float32_t` in `common.h`. The RA4C1 FPv5-SP FPU accelerates single-precision natively; double is software-emulated. City-level resolution (≈ 0.0001°) is adequate for sunrise/sunset.
- **CfgDataStore parameter-block engine** — parameters are registered by pointer into a RAM mirror struct. Writes go through a two-phase validate-then-commit cycle. Blocks carry a version counter enabling efficient change detection without polling.
- **HAL abstraction** — all hardware access is isolated in `Code/HAL/RA4C1/`. Porting to a different Renesas variant requires only a new HAL folder and toolchain file.