# Scheduler Module Design Document

**Location:** `Code/App/`  
**Files:** `Scheduler.h`, `Scheduler.c`

---

## Purpose

The Scheduler is the runtime decision engine of the Astro Timer. It is called every minute by the
bare-metal main loop and is responsible for:

1. Reading the current date/time from DateTime.
2. Detecting if the date has changed and triggering `Astro_UpdateForDate()`.
3. Evaluating the current operating mode (Auto, Random, Holiday, Manual) from CfgDataStore.
4. Applying the correct switching logic per channel:
   - Manual mode: apply stored manual channel states directly.
   - Holiday mode: suppress all automatic switching.
   - Auto mode: evaluate astronomical events and time programs.
   - Random mode: replace scheduled switching with a random interval override.
5. Driving HAL_GPIO to set physical channel outputs.

The Scheduler is the only module that writes to HAL_GPIO. All other modules either supply inputs
(CfgDataStore, DateTime, Astro) or consume outputs (LUI, HAL_Display).

---

## Design Decisions

- **Flat file, no subfolder.** Scheduler sits directly under `Code/App/` because it is the central
  integration point — its position at the top of the dependency tree makes its flat location
  intentional and immediately visible.
- **Bare-metal tick model.** `Scheduler_RunMinuteTick()` is called from the main loop when the
  RTC 1-minute interrupt flag is set. There is no RTOS, no task stack, and no preemption. The
  function must complete within the time budget of one minute.
- **Mode priority order.** Manual > Holiday > Random > Auto. Each mode check short-circuits if it
  applies, so lower-priority logic is never executed.

---

## API Reference

### `void Scheduler_Init(void)`

| | |
|---|---|
| **Purpose** | Initialises the Scheduler module. Resets any internal state (last-known date, channel states, random interval counters). |
| **Input** | None |
| **Output / Return** | None |
| **Side effects** | None at hardware level. Prepares internal state for the first `Scheduler_RunMinuteTick()` call. |
| **Call context** | Called once at system startup from `main()`, after all dependency modules (DateTime, CfgDataStore, Astro) have been initialised. |

---

### `void Scheduler_RunMinuteTick(void)`

| | |
|---|---|
| **Purpose** | Evaluates all switching rules and updates channel outputs. This is the core product function of the Astro Timer. |
| **Input** | None (reads state from DateTime, CfgDataStore, Astro internally) |
| **Output / Return** | None |
| **Side effects** | May call `HAL_GPIO_SetChannel()` to change physical relay states. May call `Astro_UpdateForDate()` if the date has changed. |
| **Call context** | Called from the bare-metal main loop exactly once per minute, triggered by the RTC periodic interrupt flag. |

**Internal decision chain:**

```
DateTime_Get(...)
    |
    +-- date changed? --> Astro_UpdateForDate(year, month, day)
    |
    v
ParameterBlock_UpdateBlock(ASTRO_BLOCK_MODE, ...)
    |
    +-- MODE_MANUAL  --> apply manual channel states --> HAL_GPIO_SetChannel() --> return
    |
    +-- MODE_HOLIDAY --> suppress all switching --> return
    |
    +-- MODE_AUTO / MODE_RANDOM:
            |
            +-- evaluate astro events (sunrise/sunset + offset vs. current minute)
            |       ParameterBlock_GetBlock(ASTRO_BLOCK_LOCATION, ...)
            |       Astro_GetSunriseMinutes() / Astro_GetSunsetMinutes()
            |       ParameterBlock_GetBlock(ASTRO_BLOCK_CHANNEL_CONFIG, ...)
            |
            +-- evaluate time programs (TODO: Programs module)
            |
            +-- MODE_RANDOM --> randomise resolved state with interval override
            |
            v
        HAL_GPIO_SetChannel(channel, resolvedState)
```

---

## Dependencies

| Module | Usage |
|---|---|
| `DateTime` | Reads current date/time each tick |
| `CfgDataStore` (ParameterBlock) | Reads mode, location/offset, channel config via UpdateBlock |
| `Astro` | Triggers daily calculation; reads sunrise/sunset minutes |
| `HAL_GPIO` | Drives physical relay outputs |

---

## Data Flow

```
[RTC 1-minute interrupt sets g_OneMinuteFlag in main.c]
        |
        v
Scheduler_RunMinuteTick()
        |
        +----> DateTime_Get() ---------> current time/date
        |
        +----> ParameterBlock_UpdateBlock(MODE) --> ModeType
        |
        +----> if date changed:
        |          ParameterBlock_GetBlock(LOCATION) --> lat/lon/tz/offset
        |          Astro_UpdateForDate(year, month, day)
        |
        +----> Astro_GetSunriseMinutes() / Astro_GetSunsetMinutes()
        |
        +----> ParameterBlock_UpdateBlock(CHANNEL_CONFIG) --> ChannelAstroMode[2]
        |
        +----> resolve ON/OFF per channel
        |
        v
HAL_GPIO_SetChannel(0, state0)
HAL_GPIO_SetChannel(1, state1)
```
