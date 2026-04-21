# CfgDataStore Module Design Document

**Location:** `Code/App/CfgDataStore/`  
**Files:** `Types.h`, `ParameterBlock.h`, `ParameterBlock.c`, `WriteChecks.h`, `WriteChecks.c`,
`Private.h`

---

## Purpose

CfgDataStore is the configuration data management layer. It owns a single RAM mirror of all
user-configurable parameters and provides:

- **Validated writes** — every write passes through a registered write-check callback before being
  applied.
- **Atomic block operations** — a two-phase commit ensures a group of related parameters is either
  fully written or not written at all.
- **Version-based change detection** — each block carries a version counter; callers hold a
  `LocalBlock_t` token and can detect changes without polling.
- **NVM persistence** — the RAM mirror is the primary storage; HAL_NVM is called to persist changes
  (stubbed until HAL_NVM is implemented).

---

## File Responsibilities

| File | Role |
|---|---|
| `Types.h` | All shared types: enums, structs, parameter/block IDs |
| `ParameterBlock.h` | Public API + `CFGPARAM` / `CFGBLOCK` helper macros |
| `ParameterBlock.c` | RAM mirror, parameter/block tables, engine implementation |
| `WriteChecks.h` | Public `WriteCheck_t` instances for each parameter |
| `WriteChecks.c` | Range/validity check functions + `Private_WriteCheck` dispatcher |
| `Private.h` | Internal-only declaration of `Private_WriteCheck`; not for external use |

---

## Key Types (from `Types.h`)

### `WriteError_t`
Return code for all write operations.

| Value | Meaning |
|---|---|
| `WRITE_ERROR_NONE` | Success — value accepted and written |
| `WRITE_WARNING_NO_CHANGE` | Value identical to current — no write performed |
| `WRITE_ERROR_INVALID` | Out of range or invalid enum value |
| `WRITE_ERROR_NULL_SOURCE` | Null pointer passed as source |
| `WRITE_ERROR_SIZE_MISMATCH` | `size` argument does not match the registered parameter size |
| `WRITE_ERROR_PARAM_NOT_FOUND` | Parameter ID not registered in the engine |

### `LocalBlock_t`
Caller-held change-detection token.

| Field | Type | Description |
|---|---|---|
| `version` | `uint32_t` | Last block version seen by this caller. Initialise to `0`. |
| `pBaseData` | `void*` | Pointer to a caller-allocated block-shaped struct. |

### Block-shaped structs

| Struct | Block ID | Fields |
|---|---|---|
| `LocationBlock_t` | `ASTRO_BLOCK_LOCATION` | `latitude`, `longitude`, `timezone_offset_minutes`, `astro_offset_minutes` |
| `ModeBlock_t` | `ASTRO_BLOCK_MODE` | `mode` (`ModeType`) |
| `ChannelConfigBlock_t` | `ASTRO_BLOCK_CHANNEL_CONFIG` | `channelAstroMode[2]` (`ChannelAstroMode`) |

---

## API Reference

### `void ParameterBlock_Init(void)`

| | |
|---|---|
| **Purpose** | Initialises the engine: loads defaults into the RAM mirror, registers all parameters and blocks into their linked lists, then restores persisted values from HAL_NVM (TODO). |
| **Input** | None |
| **Output / Return** | None |
| **Call context** | Called once at system startup, before any Get/Set calls. |

---

### `void ParameterBlock_DefaultParameters(void)`

| | |
|---|---|
| **Purpose** | Resets all parameters in the RAM mirror to their compiled-in default values. Does not write to NVM. |
| **Input** | None |
| **Output / Return** | None |
| **Call context** | Called internally by `ParameterBlock_Init()`. Also useful in test code to restore a known state between test cases. |

---

### `WriteError_t ParameterBlock_SetParam(AstroTimer_Param_t param, void const* pSource, uint32_t size)`

| | |
|---|---|
| **Purpose** | Validates and writes a single parameter to the RAM mirror. On success increments the version counter of every block that contains this parameter. |
| **Input** | `param` — parameter ID from `AstroTimer_Param_t` |
| | `pSource` — pointer to the new value |
| | `size` — byte size of `*pSource`; must match the registered parameter size |
| **Output / Return** | `WriteError_t` — `WRITE_ERROR_NONE` on success, error code otherwise |
| **Side effects** | Updates RAM mirror; increments containing block versions; calls HAL_NVM_Write (TODO). |
| **Call context** | Called by LUI when the user saves an individual setting, or by the Scheduler/other modules that need to persist a single value. |

---

### `bool ParameterBlock_GetParam(AstroTimer_Param_t param, void* pDest, uint32_t size)`

| | |
|---|---|
| **Purpose** | Reads a single parameter from the RAM mirror into the caller's buffer. |
| **Input** | `param` — parameter ID from `AstroTimer_Param_t` |
| | `pDest` — pointer to destination buffer |
| | `size` — byte size of `*pDest`; must match the registered parameter size |
| **Output / Return** | `bool` — `true` on success, `false` if not found or size mismatch |
| **Side effects** | None |

---

### `void ParameterBlock_GetBlock(AstroTimer_Block_t block, LocalBlock_t* pLocalBlock)`

| | |
|---|---|
| **Purpose** | Copies all parameters belonging to `block` into `pLocalBlock->pBaseData` and updates `pLocalBlock->version` to match the block's current version. |
| **Input** | `block` — block ID from `AstroTimer_Block_t` |
| | `pLocalBlock` — caller's token; `pBaseData` must point to the correct block-shaped struct |
| **Output / Return** | None (data written into `*pLocalBlock->pBaseData`) |
| **Side effects** | Updates `pLocalBlock->version`. |
| **Call context** | Called to read a complete block of related parameters at once. |

---

### `WriteError_t ParameterBlock_SetBlock(AstroTimer_Block_t block, LocalBlock_t const* pLocalBlock)`

| | |
|---|---|
| **Purpose** | Two-phase atomic write of all parameters in a block. Phase 1 validates every parameter; Phase 2 commits all values only if all validations pass. Restores `block->version` to `pLocalBlock->version` after commit (useful for NVM reload). |
| **Input** | `block` — block ID from `AstroTimer_Block_t` |
| | `pLocalBlock` — source data; `pBaseData` must point to the block-shaped struct |
| **Output / Return** | `WriteError_t` — `WRITE_ERROR_NONE` if all parameters accepted, first error code otherwise |
| **Side effects** | Updates RAM mirror for all parameters in the block; sets block version to `pLocalBlock->version`. |

---

### `bool ParameterBlock_UpdateBlock(AstroTimer_Block_t block, LocalBlock_t* pLocalBlock)`

| | |
|---|---|
| **Purpose** | Efficient change-detection. Compares `block->version` with `pLocalBlock->version`. If they differ, calls `ParameterBlock_GetBlock()` to refresh `pLocalBlock` and returns `true`. If identical, returns `false` immediately without any data copy. |
| **Input** | `block` — block ID from `AstroTimer_Block_t` |
| | `pLocalBlock` — caller's token, initialised with `version = 0` on first use |
| **Output / Return** | `bool` — `true` if block was refreshed (data has changed since last call), `false` if nothing changed |
| **Side effects** | Updates `pLocalBlock` if changed. |
| **Call context** | Called by Scheduler or other consumers each minute tick to check if configuration has been updated by LUI. |

---

### `void ParameterBlock_AddParameter(Parameter_t* pParam)` / `void ParameterBlock_AddBlock(ParameterBlock_t* pBlock)`

| | |
|---|---|
| **Purpose** | Registers an additional parameter or block into the engine's linked lists at runtime. Enables extensibility without modifying the core engine. |
| **Input** | Pointer to a statically allocated `Parameter_t` or `ParameterBlock_t` |
| **Output / Return** | None |
| **Call context** | Called internally during `ParameterBlock_Init()` for all built-in parameters. Can also be called by application extensions. |

---

## Write-Check System (`WriteChecks.h` / `WriteChecks.c`)

Each configurable parameter has a `WriteCheck_t` instance registered in the parameter table.
`WriteChecks.c` implements the validation functions and exposes them as `const` global instances.

| Instance | Validated type | Accepted range |
|---|---|---|
| `g_WriteCheck_Latitude` | `double` | −90.0 to +90.0 degrees |
| `g_WriteCheck_Longitude` | `double` | −180.0 to +180.0 degrees |
| `g_WriteCheck_TimezoneOffset` | `int16_t` | −720 to +840 minutes (UTC−12 to UTC+14) |
| `g_WriteCheck_AstroOffset` | `int16_t` | −120 to +120 minutes |
| `g_WriteCheck_Mode` | `ModeType` | `MODE_AUTO`, `MODE_RANDOM`, `MODE_HOLIDAY`, `MODE_MANUAL` |
| `g_WriteCheck_ChannelAstroMode` | `ChannelAstroMode` | `CHANNEL_ASTRO_DISABLED`, `CHANNEL_ASTRO_SUNSET_ON`, `CHANNEL_ASTRO_SUNRISE_ON` |

`Private_WriteCheck()` (declared in `Private.h`, implemented in `WriteChecks.c`) is the internal
dispatcher called by `ParameterBlock.c` before every write. External code must not call it directly.

---

## Change Detection Pattern

```c
// At startup — initialise caller token
LocationBlock_t locData;
LocalBlock_t    loc = { 0u, &locData };

// Each minute tick
if (ParameterBlock_UpdateBlock(ASTRO_BLOCK_LOCATION, &loc))
{
    // locData is now fresh — re-read latitude, longitude etc.
}
```

The initial `version = 0` always triggers the first update because every block is initialised to
`version = 0xEFFFFFFFU` in the parameter table.

---

## Data Flow

```
LUI (user input)
    |
    v
ParameterBlock_SetParam / ParameterBlock_SetBlock
    |--- WriteCheck (validate range/enum)
    |--- memcpy to RAM mirror
    |--- Increment block version(s)
    |--- HAL_NVM_Write (TODO)
    |
    v
ParameterBlock_UpdateBlock  <-- polled by Scheduler each minute
    |--- version match? no -> ParameterBlock_GetBlock -> caller's LocalBlock_t
```
