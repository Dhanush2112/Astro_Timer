# LUI Module Design Document

**Location:** `Code/App/LUI/`  
**Files:** `LUI.h`, `LUI.c`

---

## Purpose

LUI (Local User Interface) is the application-layer module responsible for all user-facing
interaction: button/input scanning, display/LED updates, and the menu navigation state machine.

Development of this module is **deferred**. The current implementation is a stub that compiles and
links but performs no action. The API is fixed so that `main.c` and the main loop can already
reference LUI without modification when the real implementation is added.

---

## Design Decisions

- **Polled, not interrupt-driven.** `LUI_Poll()` is called every main-loop iteration (not every
  minute tick), giving it a faster response rate than the Scheduler for immediate user feedback.
- **Writes config via CfgDataStore.** All user setting changes made through the menu are applied
  through `ParameterBlock_SetParam()` / `ParameterBlock_SetBlock()`. LUI never writes to hardware
  directly except through `HAL_Display`.
- **Reads time via DateTime.** Time display and time-set operations go through `DateTime_Get()` /
  `DateTime_Set()`, not directly through HAL_RTC.
- **Stub-safe.** Empty stub implementations ensure zero impact on system behaviour during
  development phases that do not yet require the UI.

---

## API Reference

### `void LUI_Init(void)`

| | |
|---|---|
| **Purpose** | Initialises the LUI module. Configures display peripherals and sets the initial display state. |
| **Input** | None |
| **Output / Return** | None |
| **Side effects** | Calls `HAL_Display_Init()` to configure LED/display output pins. Initialises the menu state machine to its idle/home state. |
| **Call context** | Called once at system startup from `main()`, after HAL peripherals have been initialised. |

---

### `void LUI_Poll(void)`

| | |
|---|---|
| **Purpose** | Scans inputs, updates the display, and advances the menu state machine. Must be non-blocking; must return quickly on every call. |
| **Input** | None (reads button/input state from HAL internally) |
| **Output / Return** | None |
| **Side effects** | May call `ParameterBlock_SetParam()` / `ParameterBlock_SetBlock()` when the user confirms a setting change. May call `DateTime_Set()` when the user sets the clock. May call `HAL_Display_SetChannelLed()` / `HAL_Display_SetModeLed()` to update indicators. |
| **Call context** | Called from the bare-metal main loop on every iteration (not gated by the minute timer). |

---

## Dependencies

| Module | Usage |
|---|---|
| `CfgDataStore` (ParameterBlock) | Reads current config for display; writes user changes |
| `DateTime` | Reads current time for display; writes new time on user set |
| `HAL_Display` | Drives status LEDs and display indicators |

---

## Menu Features (to be implemented)

- Location configuration (latitude, longitude, timezone)
- Astro offset setting (±120 min)
- Channel astronomical mode selection per channel
- Operating mode selection (Auto / Random / Holiday / Manual)
- Holiday period start/end date entry
- Manual channel ON/OFF override
- Clock set / time correction adjustment
- Program management (add / remove daily and yearly programs)
