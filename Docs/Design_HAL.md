# HAL Module Design Document

**Location:** `Code/HAL/RA4C1/`  
**Sub-modules:** `HAL_GPIO/`, `HAL_RTC/`, `HAL_NVM/`, `HAL_Display/`  
**Platform entry:** `main.c`

---

## Purpose

The Hardware Abstraction Layer (HAL) isolates all RA4C1-specific peripheral register access behind a
platform-neutral C API. Application modules (`DateTime`, `CfgDataStore`, `Scheduler`, `LUI`) never
include MCU vendor headers; they only include HAL headers. Porting to a different MCU requires
changes only inside `Code/HAL/`.

`main.c` is the platform entry point. It owns clock and peripheral initialisation, starts all
application modules, and runs the bare-metal main loop.

---

## `main.c`

### Purpose
Platform entry point. Initialises hardware peripherals in dependency order, then initialises
application modules, then enters the bare-metal main loop.

### Main loop structure

```c
while (1)
{
    if (g_OneMinuteFlag)        // set by RTC 1-minute periodic interrupt
    {
        Scheduler_RunMinuteTick();
        g_OneMinuteFlag = false;
    }
    LUI_Poll();
}
```

### `void HAL_RTC_MinuteCallback(void)`

| | |
|---|---|
| **Purpose** | RTC periodic interrupt handler. Sets the `g_OneMinuteFlag` volatile flag to signal the main loop that a minute has elapsed. |
| **Input** | None (called by hardware interrupt vector) |
| **Output / Return** | None |
| **Side effects** | Sets `g_OneMinuteFlag = true`. |
| **Call context** | Called automatically by the RA4C1 RTC peripheral interrupt at the 1-minute interval. |

---

## HAL_GPIO

**Files:** `HAL_GPIO.h`, `HAL_GPIO.c`

Controls the two relay output channels. Called exclusively by the Scheduler.

### `void HAL_GPIO_Init(void)`

| | |
|---|---|
| **Purpose** | Configures relay output GPIO pins on the RA4C1 as outputs and drives them to the OFF (safe) state. |
| **Input** | None |
| **Output / Return** | None |
| **Call context** | Called once from `main()` during hardware initialisation. |

---

### `void HAL_GPIO_SetChannel(uint8_t channel, bool state)`

| | |
|---|---|
| **Purpose** | Sets the physical relay output for the specified channel. |
| **Input** | `channel` — channel index (0 or 1) |
| | `state` — `true` = relay ON (energised), `false` = relay OFF |
| **Output / Return** | None |
| **Side effects** | Writes to RA4C1 GPIO output register, changing the physical relay state. |
| **Call context** | Called by `Scheduler_RunMinuteTick()` after resolving the final channel state. |

---

### `bool HAL_GPIO_GetChannel(uint8_t channel)`

| | |
|---|---|
| **Purpose** | Reads back the current logical output state of the specified channel. |
| **Input** | `channel` — channel index (0 or 1) |
| **Output / Return** | `bool` — `true` if the channel output is currently ON, `false` if OFF |
| **Side effects** | None |
| **Call context** | Called by LUI to display current channel status, or by the Scheduler for state validation. |

---

## HAL_RTC

**Files:** `HAL_RTC.h`, `HAL_RTC.c`

Interfaces directly with the RA4C1 hardware RTC peripheral. Only `DateTime.c` includes this header.

### `void HAL_RTC_Init(void)`

| | |
|---|---|
| **Purpose** | Configures the RA4C1 RTC peripheral: enables backup power (VBAT), starts the oscillator, and sets up the 1-minute periodic interrupt. |
| **Input** | None |
| **Output / Return** | None |
| **Call context** | Called once from `main()` during hardware initialisation. |

---

### `void HAL_RTC_GetTime(int16_t* year, uint8_t* month, uint8_t* day, uint8_t* hour, uint8_t* minute, uint8_t* second)`

| | |
|---|---|
| **Purpose** | Reads the current date and time from the RA4C1 RTC registers. |
| **Input** | All parameters are output pointers |
| **Output** | `*year` — 4-digit year |
| | `*month` — month 1–12 |
| | `*day` — day 1–31 |
| | `*hour` — hour 0–23 |
| | `*minute` — minute 0–59 |
| | `*second` — second 0–59 |
| **Return** | None |
| **Call context** | Called only from `DateTime_Get()`. |

---

### `void HAL_RTC_SetTime(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)`

| | |
|---|---|
| **Purpose** | Writes a new date/time to the RA4C1 RTC registers. |
| **Input** | `year`, `month`, `day`, `hour`, `minute`, `second` — new date/time values |
| **Output / Return** | None |
| **Side effects** | Updates hardware RTC registers. The RTC continues counting from the new value immediately. |
| **Call context** | Called only from `DateTime_Set()`. |

---

## HAL_NVM

**Files:** `HAL_NVM.h`, `HAL_NVM.c`

Provides address-based read/write access to the RA4C1 non-volatile data flash. Only
`CfgDataStore/ParameterBlock.c` calls these functions.

### `void HAL_NVM_Init(void)`

| | |
|---|---|
| **Purpose** | Initialises the RA4C1 data flash controller. Prepares the flash memory for read and write operations. |
| **Input** | None |
| **Output / Return** | None |
| **Call context** | Called once from `main()`, before `ParameterBlock_Init()`. |

---

### `void HAL_NVM_Read(uint32_t address, void* data, uint32_t length)`

| | |
|---|---|
| **Purpose** | Reads `length` bytes from NVM starting at `address` into the buffer pointed to by `data`. |
| **Input** | `address` — byte offset within the NVM data flash region |
| | `data` — destination buffer (must be at least `length` bytes) |
| | `length` — number of bytes to read |
| **Output / Return** | None (data written to `*data`) |
| **Side effects** | None to system state |
| **Call context** | Called by `ParameterBlock_Init()` to restore persisted configuration at startup. |

---

### `void HAL_NVM_Write(uint32_t address, const void* data, uint32_t length)`

| | |
|---|---|
| **Purpose** | Writes `length` bytes from `data` to NVM starting at `address`. Performs sector erase internally if required by the flash hardware. |
| **Input** | `address` — byte offset within the NVM data flash region |
| | `data` — source buffer (must be at least `length` bytes) |
| | `length` — number of bytes to write |
| **Output / Return** | None |
| **Side effects** | Erases and programs the corresponding data flash sector on the RA4C1. Execution may block for the duration of the flash operation. |
| **Call context** | Called by `ParameterBlock_SetParam()` / `ParameterBlock_SetBlock()` after a validated write to the RAM mirror. |

---

## HAL_Display

**Files:** `HAL_Display.h`, `HAL_Display.c`

Controls status LEDs and display indicators. Called by LUI and optionally by the Scheduler for
immediate mode/channel status feedback.

### `void HAL_Display_Init(void)`

| | |
|---|---|
| **Purpose** | Configures indicator LED GPIO pins on the RA4C1 as outputs and turns all LEDs off. |
| **Input** | None |
| **Output / Return** | None |
| **Call context** | Called once from `LUI_Init()`. |

---

### `void HAL_Display_SetChannelLed(uint8_t channel, bool on)`

| | |
|---|---|
| **Purpose** | Turns the status LED for the specified output channel on or off. |
| **Input** | `channel` — channel index (0 or 1) |
| | `on` — `true` = LED on, `false` = LED off |
| **Output / Return** | None |
| **Side effects** | Writes to RA4C1 GPIO output register controlling the LED pin. |
| **Call context** | Called by `LUI_Poll()` to reflect current channel relay state on the panel indicators. |

---

### `void HAL_Display_SetModeLed(uint8_t mode, bool on)`

| | |
|---|---|
| **Purpose** | Turns the mode indicator LED for the specified operating mode on or off. |
| **Input** | `mode` — mode index corresponding to `ModeType` values (0=Auto, 1=Random, 2=Holiday, 3=Manual) |
| | `on` — `true` = LED on, `false` = LED off |
| **Output / Return** | None |
| **Side effects** | Writes to RA4C1 GPIO output register controlling the mode LED pin. |
| **Call context** | Called by `LUI_Poll()` to display the currently active operating mode on the panel. |

---

## HAL Dependency Summary

```
App Layer          HAL Layer
---------          ---------
DateTime    <----> HAL_RTC
CfgDataStore<----> HAL_NVM
Scheduler   <----> HAL_GPIO
LUI         <----> HAL_Display

main.c initialises: HAL_NVM → HAL_RTC → HAL_GPIO → HAL_Display
                    then:     ParameterBlock_Init → DateTime_Init → Astro_Init
                              → Scheduler_Init → LUI_Init
```
