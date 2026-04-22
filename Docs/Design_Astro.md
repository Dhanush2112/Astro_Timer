# Astro Module Design Document

**Location:** `Code/App/Astro/`  
**Files:** `Astro.h`, `Astro.c`  
**Algorithm:** NOAA Solar Calculator (Meeus-based)

---

## Purpose

The Astro module is a self-contained algorithm module. It calculates local sunrise and sunset times
(expressed as minutes from midnight) for a given calendar date, latitude, longitude, and timezone.
It holds no persistent state beyond the last calculated result and the configured location. It has
no dependency on any other application module — it is a pure computation unit.

The Scheduler calls `Astro_SetLocation()` once at startup (and again on location change), then calls
`Astro_UpdateForDate()` once per day at midnight or on date change, and reads the resulting times
each minute via `Astro_GetSunriseMinutes()` / `Astro_GetSunsetMinutes()`.

---

## Algorithm Selection

### Candidate Algorithms

**1. PSA — Practical Solar Algorithm (Blanco-Muriel et al., 2001)**  
A low-complexity algorithm targeting ±0.5° in solar azimuth and elevation. It uses a compact
closed-form expression with no tables. Its formally validated range is 1999–2015; accuracy degrades
outside this window. No sunrise/sunset refinement is included — rise/set times must be derived
from the position calculation.

**2. NOAA Solar Calculator (Meeus-based)**  
Developed by NOAA's Earth System Research Laboratory, based on Jean Meeus "Astronomical Algorithms"
(2nd ed.). Computes solar position and rise/set times with ±1–2 minute accuracy. Valid range
1950–2050. Widely used in industry and validated against USNO ephemeris tables. NOAA publishes
a reference JavaScript spreadsheet that serves as an exact oracle for testing.

**3. Walraven (1978)**  
An older simplified formula aimed at fast computation. Accuracy is ±0.5°–1° in position, which
translates to ±5–15 minutes in rise/set time depending on latitude and season. No authoritative
reference implementation or validated oracle exists for independent test verification.

**4. USNO Simplified (U.S. Naval Observatory)**  
A simplified algorithm published by the USNO for computing approximate sunrise/sunset. Achieves
±2–5 minute accuracy at mid-latitudes. Similar in structure to NOAA but with fewer correction
terms. Not as widely used in embedded products and lacks a convenient public reference calculator
for generating test vectors.

**5. SPA — Solar Position Algorithm (Reda & Andreas, NREL, 2003/2008)**  
The most accurate algorithm of the five: ±0.0003° in position, ±0.5–1 minute in rise/set, valid
from year −2000 to +6000. Based on the full Meeus periodic-term tables (63 nutation rows, 5 sets
of heliocentric tables). Requires `double` (64-bit) arithmetic throughout due to Julian Day
arithmetic involving catastrophic cancellation in single precision.

---

### Hardware Constraints (RA4C1)

The target MCU defines hard constraints that directly affect algorithm choice:

| Constraint | Detail |
|---|---|
| **Core** | Arm Cortex-M33 |
| **FPU** | FPv5-SP — hardware single-precision only; `double` operations use IAR software library |
| **Flash** | 256 KB |
| **SRAM** | 64 KB |
| **Execution model** | Bare-metal; `Astro_UpdateForDate()` called once per day, not in a real-time loop |
| **Dynamic allocation** | None used; all storage must be static or stack |

---

### Algorithm Comparison

| Criterion | PSA | NOAA | Walraven | USNO | SPA |
|---|---|---|---|---|---|
| **Rise/set accuracy** | ±2–4 min | ±1–2 min | ±5–15 min | ±2–5 min | ±0.5–1 min |
| **City-level sufficient (±5 min)** | Yes | Yes | Marginal | Yes | Yes |
| **Valid year range** | 1999–2015 | 1950–2050 | Broad (unvalidated) | 1950–2050 | −2000 to +6000 |
| **Covers product lifetime (>2036)** | No | Yes | Unknown | Yes | Yes |
| **`double` required?** | No | No | No | No | Yes — entire pipeline |
| **HW FPU utilised?** | Yes (float32) | Yes (float32, 100%) | Yes (float32) | Yes (float32) | No (all SW emulated) |
| **Lookup tables (ROM)** | None | None | None | None | ~6–8 KB |
| **Code size (approx.)** | ~50 lines | ~130 lines | ~50 lines | ~80 lines | ~600 lines |
| **Implementation effort** | Very low | Low | Very low | Low | High |
| **Test oracle available?** | Limited | Yes — NOAA JS spreadsheet | No | Limited | Yes — NREL C reference |
| **Industry adoption** | Moderate | High | Low | Moderate | Very high (research) |

---

### Selected Algorithm: NOAA

NOAA is the best fit for this product. The rationale:

1. **Accuracy meets the requirement.** ±1–2 minute accuracy at mid-latitudes satisfies city-level
   precision. The product applies a user-configurable ±120 minute astronomical offset on top of the
   calculated time, so sub-minute algorithmic accuracy beyond ±2 min provides no user-visible
   benefit.

2. **Covers the full product lifetime.** The 1950–2050 valid range safely covers any unit
   manufactured today and in use through 2050. PSA's 1999–2015 window already lies in the past.

3. **Entirely `float32_t` — no `double` required.** The Julian Day Number is computed using pure
   integer arithmetic (`int32_t`). The century offset `JDN − 2,451,545` is an exact integer
   subtraction; the result (~9,600 for 2026) is well within `float32_t`'s exact integer range
   (2²⁴ = 16,777,216). Dividing by 36525.0f in `float32_t` gives T ≈ 0.26 with ~10⁻⁷ relative
   error, contributing less than 0.1 minute to the final result. Every FPU operation uses
   hardware single-precision, fully utilising the FPv5-SP.

4. **Fastest to implement with a verified oracle.** NOAA's public JavaScript spreadsheet
   (nrel.gov/gis/solar-calculators/) computes to the same algorithm, making it trivial to generate
   reference values for any date and location for `Test_Astro.c`.

5. **No ROM tables.** The algorithm is entirely polynomial — no const arrays in flash.

SPA was rejected because it mandates `double` throughout (all FPU operations fall back to IAR
software library on FPv5-SP), adds ~6–8 KB of lookup tables, and its ±0.5 min accuracy provides
no advantage for a 1-minute-resolution product. Walraven was rejected on accuracy grounds.

---

## NOAA Algorithm — Computation Steps

The NOAA algorithm computes solar declination and the equation of time, derives the geometric
hour angle at the standard horizon, and converts to local rise/set minutes. All floating-point
steps use `float32_t` throughout. The Julian Day Number is derived via `int32_t` arithmetic
(unsigned is not possible here — the intermediate Gregorian correction term `B` and the century
offset `JDN − 2,451,545` can both be negative). All other integer types are unsigned wherever
the value is logically non-negative.

Helper macros in `Astro.c`:
```c
#define DEG2RAD(x)   ((x) * (float32_t)(3.14159265358979323846 / 180.0))
#define RAD2DEG(x)   ((x) * (float32_t)(180.0 / 3.14159265358979323846))
```

---

### Step 1 — Julian Day Number (JDN) and Julian Century (T)

Compute the integer Julian Day Number for noon of the given date using pure `int32_t` arithmetic.
The century offset `JDN − 2,451,545` (~9,600 for year 2026) is well within `float32_t`'s exact
integer range (integers up to 2²⁴ = 16,777,216 are represented exactly). Dividing by 36525.0f
in `float32_t` yields T ≈ 0.26 with <10⁻⁷ relative error — contributing less than 0.1 minute
to the final sunrise/sunset result.

```
If month ≤ 2:
    year_adj  = year − 1
    month_adj = month + 12
Else:
    year_adj  = year
    month_adj = month

A   = year_adj / 100                         (int32_t division)
B   = 2 − A + A/4                            (int32_t division)

JDN = INT(365.25 × (year_adj + 4716))
    + INT(30.6001 × (month_adj + 1))
    + day + B − 1524                         (int32_t result — noon of given date)

T = (float32_t)(JDN − 2451545) / 36525.0f   /* int32_t subtraction → exact; then float32 divide */
```

Subtracting 0.5 from T (to adjust from noon to midnight) is omitted here; its effect on T is
0.5/36525 ≈ 1.4×10⁻⁵, which shifts sunrise/sunset by less than 0.02 minutes and is below the
algorithm's inherent accuracy floor.

---

### Step 2 — Geometric Mean Longitude of the Sun (L₀)

```
L₀ = 280.46646 + 36000.76983·T + 0.0003032·T²    (degrees)
L₀ = fmodf(L₀, 360.0f)
If L₀ < 0: L₀ += 360.0f
```

---

### Step 3 — Geometric Mean Anomaly of the Sun (M)

```
M = 357.52911 + 35999.05029·T − 0.0001537·T²    (degrees)
M = fmodf(M, 360.0f)
If M < 0: M += 360.0f
```

---

### Step 4 — Eccentricity of Earth's Orbit (e)

```
e = 0.016708634 − 0.000042037·T − 0.0000001267·T²    (dimensionless)
```

---

### Step 5 — Sun's Equation of Center (C)

```
C = (1.914602 − 0.004817·T − 0.000014·T²) · sinf(DEG2RAD(M))
  + (0.019993 − 0.000101·T)               · sinf(DEG2RAD(2·M))
  +  0.000289                              · sinf(DEG2RAD(3·M))   (degrees)
```

---

### Step 6 — Sun's True Longitude (Θ) and Apparent Longitude (λ)

```
Θ = L₀ + C                                      (degrees, Sun's true longitude)

Ω = 125.04 − 1934.136·T                          (degrees, longitude of ascending node)
λ = Θ − 0.00569 − 0.00478·sinf(DEG2RAD(Ω))     (degrees, apparent longitude)
```

---

### Step 7 — Mean and Corrected Obliquity of the Ecliptic (ε)

```
ε₀ = 23.0 + (26.0 + (21.448 − T·(46.8150 + T·(0.05620 − T·0.001813))) / 60.0) / 60.0
ε  = ε₀ + 0.00256·cosf(DEG2RAD(Ω))              (degrees, corrected obliquity)
```

---

### Step 8 — Solar Declination (δ)

```
δ = RAD2DEG(asinf(sinf(DEG2RAD(ε)) · sinf(DEG2RAD(λ))))    (degrees)
```

---

### Step 9 — Equation of Time (E)

```
y = tanf(DEG2RAD(ε / 2.0f))
y = y · y                                         (y = tan²(ε/2))

E = 4.0f · RAD2DEG(
        y · sinf(DEG2RAD(2.0f·L₀))
      − 2.0f·e · sinf(DEG2RAD(M))
      + 4.0f·e·y · sinf(DEG2RAD(M)) · cosf(DEG2RAD(2.0f·L₀))
      − 0.5f·y·y · sinf(DEG2RAD(4.0f·L₀))
      − 1.25f·e·e · sinf(DEG2RAD(2.0f·M))
    )                                              (minutes)
```

---

### Step 10 — Hour Angle at Sunrise/Sunset (HA)

The value 90.833° is the standard solar horizon angle: 90° geometric + 0.5667° atmospheric
refraction + 0.2665° solar disc radius.

```
cos_HA = (cosf(DEG2RAD(90.833f)) − sinf(DEG2RAD(φ)) · sinf(DEG2RAD(δ)))
         / (cosf(DEG2RAD(φ)) · cosf(DEG2RAD(δ)))
```

Where **φ** is observer latitude (degrees, positive North).

If `cos_HA > 1.0f`:  polar night — sun never rises. Store sentinel values and return.  
If `cos_HA < −1.0f`: polar day — sun never sets. Store sentinel values and return.

```
HA = RAD2DEG(acosf(cos_HA))    (degrees, 0° < HA < 180°)
```

---

### Step 11 — Sunrise and Sunset (local minutes from midnight)

**Solar noon (UTC minutes from midnight):**
```
Solar_Noon_UTC = 720.0f − 4.0f·σ − E
```
Where **σ** is observer longitude (degrees, positive East).

**Rise and set in UTC minutes from midnight:**
```
Sunrise_UTC = Solar_Noon_UTC − 4.0f·HA
Sunset_UTC  = Solar_Noon_UTC + 4.0f·HA
```

**Convert to local time:**
```
Sunrise_local = Sunrise_UTC + (float32_t)timezone · 60.0f
Sunset_local  = Sunset_UTC  + (float32_t)timezone · 60.0f
```

**Store as `uint16_t`:**
```
g_SunriseMinutes = (uint16_t)roundf(Sunrise_local)   clamped to [0, 1439]
g_SunsetMinutes  = (uint16_t)roundf(Sunset_local)    clamped to [0, 1439]
```

---

## Design Decisions

- **Entirely `float32_t` — no `double` anywhere.** The Julian Day Number is computed in `int32_t`
  arithmetic. The century offset `JDN − 2,451,545` is an exact integer subtraction whose result
  (~9,600) is representable without rounding in `float32_t`. The subsequent division by 36525.0f
  contributes <0.1 minute of error to the final result. Every floating-point operation in the
  module executes on the FPv5-SP hardware FPU. No `double` type is introduced; `common.h` does
  not require a `float64_t` typedef for this module.

- **`Astro_SetLocation()` stores location as module state.** Location changes only when the user
  reconfigures the device, not every day. Storing it as module-private state avoids requiring the
  Scheduler to pass it on every daily call. `Astro_UpdateForDate()` uses whichever location was
  last set.

- **Longitude sign convention: positive East.** Consistent with geographic convention and the
  NOAA Solar Noon formula: `Noon_UTC = 720 − 4·longitude − E`. Eastward locations produce earlier
  noon times, which is correct.

- **Timezone: signed integer hours.** `int8_t` covers −12 to +14 (the full valid UTC offset range).
  Fractional-hour offsets (e.g. UTC+5:30) are not supported; the product specification does not
  require them.

- **Sentinel values for polar conditions.** When `|cos_HA| > 1`, `ASTRO_SUNRISE_NONE` and
  `ASTRO_SUNSET_NONE` (both `0xFFFF` / `UINT16_MAX`) are stored. `0xFFFF` is used as the sentinel
  because sunrise/sunset minutes are bounded to [0, 1439]; any value above 1439 is unambiguously
  invalid. Callers must check for this before using the result.

- **Clamping.** At latitudes near but not at the polar circle, the computed minutes may fall
  slightly outside [0, 1439] due to rounding. These are clamped rather than saturating to a
  sentinel, as a genuine rise/set event does occur that day.

- **No ΔT correction.** Unlike SPA, the NOAA algorithm works in Universal Time directly. No ΔT
  (Terrestrial Time − UTC) term is needed.

---

## API Reference

### New constants in `Astro.h`

```c
#define ASTRO_SUNRISE_NONE  ((uint16_t)0xFFFFU)   /* Sun does not rise on this date (polar night) */
#define ASTRO_SUNSET_NONE   ((uint16_t)0xFFFFU)   /* Sun does not set  on this date (polar day)   */
```

---

### `void Astro_Init(void)`

| | |
|---|---|
| **Purpose** | Initialises the module; resets sunrise/sunset to safe defaults and clears location state. |
| **Input** | None |
| **Output / Return** | None |
| **Side effects** | Sets `g_SunriseMinutes = 360` (06:00), `g_SunsetMinutes = 1080` (18:00). Clears latitude, longitude, and timezone to zero (equator, UTC). |
| **Call context** | Called once at system startup before any other Astro function. |

---

### `void Astro_SetLocation(float32_t latitude, float32_t longitude, int8_t timezone)`

| | |
|---|---|
| **Purpose** | Stores the observer's geographic location and UTC offset for use in all subsequent `Astro_UpdateForDate()` calls. |
| **Input** | `latitude`  — observer latitude, −90.0 to +90.0 degrees (positive North) |
| | `longitude` — observer longitude, −180.0 to +180.0 degrees (positive East) |
| | `timezone`  — UTC offset in whole hours, −12 to +14 |
| **Output / Return** | None |
| **Side effects** | Updates module-private `g_Latitude`, `g_Longitude`, `g_Timezone`. Does not trigger a recalculation; the new location is used on the next `Astro_UpdateForDate()` call. |
| **Call context** | Called by the Scheduler at startup after reading location from CfgDataStore, and again any time the user updates location via LUI. |

---

### `void Astro_UpdateForDate(uint16_t year, uint8_t month, uint8_t day)`

| | |
|---|---|
| **Purpose** | Runs the NOAA calculation for the given date using the stored location and updates the internal sunrise/sunset values. |
| **Input** | `year`  — 4-digit calendar year (recommended range: 1950–2050) |
| | `month` — month of year, 1–12 |
| | `day`   — day of month, 1–31 |
| **Output / Return** | None (results stored internally, retrieved via Get functions) |
| **Side effects** | Overwrites `g_SunriseMinutes` and `g_SunsetMinutes`. If `Astro_SetLocation()` has not been called, uses 0°N, 0°E, UTC+0. |
| **Call context** | Called by the Scheduler once per day, typically at midnight or immediately after a date change is detected. |

---

### `uint16_t Astro_GetSunriseMinutes(void)`

| | |
|---|---|
| **Purpose** | Returns the sunrise time from the most recent `Astro_UpdateForDate()` call. |
| **Input** | None |
| **Output / Return** | Minutes from local midnight, 0–1439. Returns `ASTRO_SUNRISE_NONE` (`0xFFFF`) if the sun does not rise on that date at the configured location. |
| **Side effects** | None |
| **Call context** | Called by the Scheduler each minute tick. The Scheduler must check for `ASTRO_SUNRISE_NONE` (`0xFFFF`) before acting on the value. |

---

### `uint16_t Astro_GetSunsetMinutes(void)`

| | |
|---|---|
| **Purpose** | Returns the sunset time from the most recent `Astro_UpdateForDate()` call. |
| **Input** | None |
| **Output / Return** | Minutes from local midnight, 0–1439. Returns `ASTRO_SUNSET_NONE` (`0xFFFF`) if the sun does not set on that date at the configured location. |
| **Side effects** | None |
| **Call context** | Called by the Scheduler each minute tick. The Scheduler must check for `ASTRO_SUNSET_NONE` (`0xFFFF`) before acting on the value. |

---

## Internal Implementation

### Module-private state (`Astro.c`)

```c
static float32_t g_Latitude        = 0.0f;
static float32_t g_Longitude       = 0.0f;
static int8_t    g_Timezone        = 0;
static uint16_t  g_SunriseMinutes  = 360U;  /* 06:00 default */
static uint16_t  g_SunsetMinutes   = 1080U; /* 18:00 default */
```

### Static helper functions (`Astro.c` — no external linkage)

| Function | Purpose |
|---|---|
| `static int32_t noaa_julian_day_number(uint16_t year, uint8_t month, uint8_t day)` | Returns integer JDN for noon of the given date using `int32_t` arithmetic (intermediate value `B` can be negative; final JDN is always positive). |
| `static float32_t noaa_julian_century(int32_t jdn)` | Converts JDN to Julian Century T: subtracts 2,451,545 in `int32_t` (result is negative for dates before year 2000), then divides by 36525.0f in `float32_t`. `int32_t` is required here — unsigned subtraction would underflow for pre-2000 dates. |
| `static float32_t noaa_reduce_degrees(float32_t deg)` | Reduces an angle to [0°, 360°) using `fmodf`. |
| `static uint16_t noaa_clamp_minutes(float32_t minutes)` | Rounds and clamps a float minute value to [0, 1439] as `uint16_t`. |

All computation in `Astro_UpdateForDate()` uses stack-local variables only. No dynamic allocation.

---

## Edge Cases

| Condition | Behaviour |
|---|---|
| Polar night (`cos_HA > 1.0f`) | Store `ASTRO_SUNRISE_NONE` and `ASTRO_SUNSET_NONE` (`0xFFFF`); return immediately |
| Polar day (`cos_HA < −1.0f`) | Store `ASTRO_SUNRISE_NONE` and `ASTRO_SUNSET_NONE` (`0xFFFF`); return immediately |
| Computed minutes < 0 | Clamp to 0 |
| Computed minutes > 1439 | Clamp to 1439 |
| `Astro_SetLocation()` never called | Calculation proceeds at 0°N, 0°E, UTC+0 (equator) |
| Year outside 1950–2050 | No guard; formula remains valid but accuracy may degrade |

---

## Test Strategy

Reference values are generated from the **NOAA Solar Calculator JavaScript spreadsheet**
(publicly available from NOAA ESRL). The test tolerance is **±2 minutes** to account for the
`float32_t` intermediate arithmetic relative to the double-precision reference.

### Required test cases (`Test_Astro.c`)

| Test name | Date | Location | Timezone | Expected result |
|---|---|---|---|---|
| `NOAA_EquatorEquinox` | 20 Mar 2026 | 0°N, 0°E | UTC+0 | Sunrise ≈ 06:00, Sunset ≈ 18:00 |
| `NOAA_MidLatSummer` | 21 Jun 2026 | 51.5°N, 0.0°E | UTC+1 | Long day; early sunrise, late sunset |
| `NOAA_MidLatWinter` | 21 Dec 2026 | 51.5°N, 0.0°E | UTC+0 | Short day; late sunrise, early sunset |
| `NOAA_SouthernHemisphere` | 21 Jun 2026 | 33.9°S, 151.2°E | UTC+10 | Winter in Southern Hemisphere |
| `NOAA_EasternTimezone` | 15 Jan 2026 | 48.1°N, 11.6°E | UTC+1 | Munich winter — verify timezone shift |
| `NOAA_PolarNight` | 21 Dec 2026 | 70.0°N, 25.0°E | UTC+2 | Both sentinels returned (`0xFFFF`) |
| `NOAA_PolarDay` | 21 Jun 2026 | 70.0°N, 25.0°E | UTC+2 | Both sentinels returned (`0xFFFF`) |
| `NOAA_TimezoneShift` | 1 Apr 2026 | 51.5°N, 0.0°E | UTC+0 vs UTC+1 | Result differs by exactly 60 min |

All test cases call `Astro_SetLocation()` before `Astro_UpdateForDate()` to configure the location
independently of any other module.

---

## Data Flow

```
CfgDataStore (latitude, longitude, timezone, astro_offset)
        |
        v
Astro_UpdateForDate(year, month, day)  <-- called by Scheduler at midnight
        |
        v
 [internal: g_SunriseMinutes, g_SunsetMinutes]
        |
        +---> Astro_GetSunriseMinutes()  --> Scheduler
        +---> Astro_GetSunsetMinutes()   --> Scheduler
```
