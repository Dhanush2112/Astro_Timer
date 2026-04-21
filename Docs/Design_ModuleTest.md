# ModuleTest Design Document

**Location:** `Code/ModuleTest/`  
**Files:** `ModulesTest_Main.c`, `Test_Astro.c`, `Test_CfgDataStore.c`, `Test_DateTime.c`,
`Test_Scheduler.c`

---

## Purpose

ModuleTest provides a host-compilable (non-embedded) test suite for the application modules. Tests
run on a PC using the standard C runtime — no MCU hardware, no RTOS, no HAL peripherals. HAL
functions are stubbed (they compile and link but perform no hardware access), so the application
logic can be exercised in isolation.

The build is controlled by the `EMBEDDED` preprocessor guard: when `EMBEDDED` is not defined,
`main()` is provided by the test runner (`ModulesTest_Main.c`) rather than `HAL/RA4C1/main.c`.

---

## Design Decisions

- **One file per module under test.** Each `Test_<Module>.c` file contains all test functions for
  that module and a single `Test_<Module>_RunAll()` runner function. This makes it easy to add,
  remove, or skip an entire module's tests.
- **`assert()` for pass/fail.** Test failures abort immediately with a clear assertion message.
  This is intentional for early-stage development — no test framework dependency is introduced.
- **`ModulesTest_Main.c` as smoke-test.** Runs a quick integration path through all modules in
  startup order to verify they initialise and produce consistent output before the unit tests run.

---

## File Descriptions

### `ModulesTest_Main.c`

Integration smoke test. Calls `_Init()` on each module in startup order, exercises one primary
function per module, and prints results to stdout. If the smoke test runs to completion without an
assertion failure, the basic module wiring is confirmed.

**Entry point:** `modules_test_main()` (called from `main()` when `EMBEDDED` is not defined).

---

### `Test_Astro.c`

Unit tests for the Astro module.

| Function | What it tests |
|---|---|
| `Test_Astro_DefaultValues()` | After `Astro_Init()` + `Astro_UpdateForDate()`, sunrise = 360 and sunset = 1080 (placeholder), and sunrise < sunset. |
| `Test_Astro_RunAll()` | Runner — calls all Astro test functions. |

---

### `Test_CfgDataStore.c`

Unit tests for the CfgDataStore ParameterBlock engine.

| Function | What it tests |
|---|---|
| `Test_CfgDataStore_DefaultValues()` | After `ParameterBlock_Init()`, location block returns all-zero defaults and mode block returns `MODE_AUTO`. |
| `Test_CfgDataStore_SetAndGetParam()` | `ParameterBlock_SetParam()` writes a value; `ParameterBlock_GetParam()` reads it back correctly. |
| `Test_CfgDataStore_SetAndGetBlock()` | `ParameterBlock_SetBlock()` writes a full `LocationBlock_t`; `ParameterBlock_GetBlock()` returns identical data. |
| `Test_CfgDataStore_UpdateBlock()` | First call returns `true`; subsequent call with no write returns `false`; after a `SetParam` call returns `true` again. |
| `Test_CfgDataStore_WriteCheckRejectsInvalid()` | Out-of-range latitude, longitude, astro offset, and invalid enum mode all return `WRITE_ERROR_INVALID`. |
| `Test_CfgDataStore_SizeMismatch()` | Passing wrong `size` to `SetParam` returns `WRITE_ERROR_SIZE_MISMATCH`. |
| `Test_CfgDataStore_RunAll()` | Runner — calls all CfgDataStore test functions. |

---

### `Test_DateTime.c`

Unit tests for the DateTime module.

| Function | What it tests |
|---|---|
| `Test_DateTime_InitAndGet()` | After `DateTime_Init()`, `DateTime_Get()` returns the placeholder date 2026-01-01 00:00:00 without crashing. |
| `Test_DateTime_RunAll()` | Runner — calls all DateTime test functions. |

---

### `Test_Scheduler.c`

Unit tests for the Scheduler module.

| Function | What it tests |
|---|---|
| `Test_Scheduler_InitAndTick()` | `Scheduler_Init()` followed by `Scheduler_RunMinuteTick()` completes without crashing (smoke test; behaviour tests added when Scheduler logic is implemented). |
| `Test_Scheduler_RunAll()` | Runner — calls all Scheduler test functions. |

---

## Adding a New Test

1. Create `Test_<Module>.c` in `Code/ModuleTest/`.
2. Implement `Test_<Module>_<CaseName>()` functions using `assert()`.
3. Implement `Test_<Module>_RunAll()` that calls each case.
4. Add a call to `Test_<Module>_RunAll()` inside `modules_test_main()` in `ModulesTest_Main.c`.
5. Add the new file to the CMake `ModuleTest` target.

---

## Build and Run (host PC)

```sh
cmake -S . -B build -DMODULE_TEST=ON
cmake --build build
./build/ModuleTest
```

Expected output (all passing):

```
=== ModulesTest_Main for Astro_Timer ===
[Astro]    Sunrise: 360 min, Sunset: 1080 min
[CfgStore] lat=0.000000 lon=0.000000 tz=0 offset=0
[DateTime] 2026-01-01 00:00:00
[Scheduler] RunMinuteTick() completed
=== All smoke tests passed ===
[PASS] Test_Astro_DefaultValues
[PASS] Test_CfgDataStore_DefaultValues
...
All unit tests passed
```
