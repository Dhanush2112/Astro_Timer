/* ParameterBlockTypes.h
 * All shared data types for the CfgDataStore parameter-block system.
 *
 * Includes:
 *   - Write-check infrastructure types (WriteError_t, WriteCheck_t, Parameter_t)
 *   - Block/parameter registry types  (BlockParameter_t, ParameterBlock_t, LocalBlock_t)
 *   - Project block and parameter ID enumerations
 *   - Domain data types and block-shaped structs
 */

#ifndef PARAMETERBLOCK_TYPES_H
#define PARAMETERBLOCK_TYPES_H

#include "common.h"

/* =========================================================================
 * Utility
 * ========================================================================= */

#define PARAM_NUM_ELEM(arr) (sizeof(arr) / sizeof((arr)[0]))

/* =========================================================================
 * Limits
 * ========================================================================= */

#define CHANNEL_COUNT 2u
#define MAX_PROGRAMS  100u

/* =========================================================================
 * Domain types
 * ========================================================================= */

typedef enum _OperatingMode_t
{
  MODE_AUTO = 0,
  MODE_RANDOM,
  MODE_HOLIDAY,
  MODE_MANUAL
} OperatingMode_t;

typedef enum _ChannelMode_t
{
  CHANNEL_MODE_DISABLED = 0,
  CHANNEL_MODE_SUNSET_ON, /* Sunset → ON,  Sunrise → OFF */
  CHANNEL_MODE_SUNRISE_ON /* Sunrise → ON, Sunset  → OFF */
} ChannelMode_t;
  
typedef enum _ProgramType_t
{
  PROGRAM_DAILY = 0,
  PROGRAM_YEARLY
} ProgramType_t;

/* =========================================================================
 * Write-check infrastructure
 * ========================================================================= */

/* Forward declaration needed for WriteCheckFn_t callback signature */
typedef struct _Parameter_t Parameter_t;

typedef enum _WriteError_t
{
  WRITE_ERROR_NONE = 0,       /* Success — value accepted and written     */
  WRITE_WARNING_NO_CHANGE,    /* Value identical to current — no write    */
  WRITE_ERROR_INVALID,        /* Value is not a valid enum/out of range   */
  WRITE_ERROR_NULL_SOURCE,    /* Null pointer passed as source            */
  WRITE_ERROR_SIZE_MISMATCH,  /* Size argument does not match param size  */
  WRITE_ERROR_PARAM_NOT_FOUND /* Parameter ID not registered              */
} WriteError_t;

typedef WriteError_t (*WriteCheckFn_t)(Parameter_t const * const pParam,
                                       void const * const pSource);

typedef struct
{
  WriteCheckFn_t pFn; /* NULL = no validation required */
} WriteCheck_t;

/* =========================================================================
 * Parameter descriptor  (one per configurable value)
 * ========================================================================= */

typedef struct _Parameter_t
{
  uint32_t index;                  /* AstroTimer_Param_t cast to uint32_t     */
  uint32_t size;                   /* Byte size of the value                  */
  void *pData;                     /* Pointer into the RAM mirror             */
  Parameter_t *pNextParam;         /* Intrusive linked-list next pointer      */
  WriteCheck_t const *pWriteCheck; /* NULL = no validation                    */
} Parameter_t;

/* =========================================================================
 * Block infrastructure
 * ========================================================================= */

/* Maps one parameter into a block-shaped struct (index + offset within struct) */
typedef struct _BlockParameter_t
{
  uint32_t index;  /* AstroTimer_Param_t cast to uint32_t          */
  uint32_t offset; /* offsetof(BlockStruct, member)                */
} BlockParameter_t;

/* Block descriptor — groups parameters; tracks changes via version counter */
typedef struct _ParameterBlock_t
{
  uint32_t blockID;
  uint32_t version; /* Incremented on every write to any param   */
  uint32_t numParameters;
  BlockParameter_t const *pParameters;
  struct _ParameterBlock_t *pNextBlock;
} ParameterBlock_t;

/* Caller-held token: version for change detection + buffer for block data */
typedef struct _LocalBlock_t
{
  uint32_t version; /* Last version seen by this caller; init to 0         */
  void *pBaseData;  /* Pointer to a caller-allocated block-shaped struct   */
} LocalBlock_t;

/* =========================================================================
 * Block IDs
 * ========================================================================= */

typedef enum _Block_t
{
  BLOCK_LOCATION = 0x1000U,
  BLOCK_MODE = 0x1001U,
  BLOCK_CHANNEL_CONFIG = 0x1002U
} Block_t;

/* =========================================================================
 * Parameter IDs
 * ========================================================================= */

typedef enum _AstroTmrParameters_t 
{
  PARAM_LATITUDE = 0U,
  PARAM_LONGITUDE = 1U,
  PARAM_TIMEZONE_OFFSET = 2U,
  PARAM_ASTRO_OFFSET = 3U,
  PARAM_MODE = 4U,
  PARAM_CH0_ASTRO_MODE = 5U,
  PARAM_CH1_ASTRO_MODE = 6U,
  PARAM_COUNT = 7U
} AstroTmrParameters_t;

/* =========================================================================
 * Block-shaped structs  (used as pBaseData inside LocalBlock_t)
 * ========================================================================= */

typedef struct _LocationBlock_t
{
  float32_t latitude;
  float32_t longitude;
  int16_t timezone_offset_minutes;
  int16_t astro_offset_minutes;
} LocationBlock_t;

typedef struct _ModeBlock_t
{
  OperatingMode_t mode;
} ModeBlock_t;

typedef struct _ChannelConfigBlock_t
{
  ChannelMode_t channelMode[CHANNEL_COUNT];
} ChannelConfigBlock_t;

#endif /* PARAMETERBLOCK_TYPES_H */
