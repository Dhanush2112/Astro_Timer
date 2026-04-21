/* CfgDataStore_ParameterBlock.c
 * Parameter-block engine: RAM mirror, parameter and block registration,
 * get/set with two-phase commit, and version-based change detection.
 *
 * Architecture (mirrors the reference project pattern)
 * ----------------------------------------------------
 *  m_CfgParams       — the single flat RAM mirror struct for all parameters.
 *  m_Params[]        — static array of Parameter_t; each entry points directly
 *                      into a field of m_CfgParams and carries its write-check.
 *  m_Blocks[]        — static array of ParameterBlock_t; each groups a set of
 *                      parameters and holds a version counter.
 *  m_pParamHead /
 *  m_pBlockHead      — heads of the intrusive linked lists built in Init().
 *
 * SetParam / SetBlock two-phase commit
 * -------------------------------------
 *  Phase 1 (validate): run Private_WriteCheck with write=false
 *                      for every parameter in the batch.
 *  Phase 2 (commit):   if all validations pass, memcpy each value and
 *                      increment the version of every containing block.
 *
 * Change detection (UpdateBlock)
 * --------------------------------
 *  Each ParameterBlock_t carries a version counter initialised to 0xEFFFFFFFU.
 *  The caller's LocalBlock_t starts with version=0U.  On the first
 *  UpdateBlock call the versions differ, so data is copied and the caller's
 *  version is updated to match.  Subsequent calls return false until another
 *  write increments the block's version again.
 */

#include "ParameterBlock.h"

#include "Private.h"
#include "WriteChecks.h"

/* =========================================================================
 * RAM mirror
 * ========================================================================= */

typedef struct
{
  float32_t latitude;
  float32_t longitude;
  int16_t timezone_offset_minutes;
  int16_t astro_offset_minutes;
  OperatingMode_t mode;
  ChannelMode_t channelAstroMode[CHANNEL_COUNT];
} AstroTimerCfg_t;

static AstroTimerCfg_t m_CfgParams; /* NOTE: must be declared before m_Params[] */

/* =========================================================================
 * Default values
 * ========================================================================= */

static AstroTimerCfg_t const m_CfgParamsDefault = {
  /* latitude */ 0.0f,
  /* longitude */ 0.0f,
  /* timezone_offset_minutes */ 0,
  /* astro_offset_minutes */ 0,
  /* mode */ MODE_AUTO,
  /* channelAstroMode */ {CHANNEL_MODE_DISABLED, CHANNEL_MODE_DISABLED}};

/* =========================================================================
 * Parameter table  (CFGPARAM macro references m_CfgParams declared above)
 * ========================================================================= */

static Parameter_t m_Params[] =
{
  { CFGPARAM(PARAM_LATITUDE,        latitude,                      &g_WriteCheck_Latitude)         },
  { CFGPARAM(PARAM_LONGITUDE,       longitude,                     &g_WriteCheck_Longitude)        },
  { CFGPARAM(PARAM_TIMEZONE_OFFSET, timezone_offset_minutes,       &g_WriteCheck_TimezoneOffset)   },
  { CFGPARAM(PARAM_ASTRO_OFFSET,    astro_offset_minutes,          &g_WriteCheck_AstroOffset)      },
  { CFGPARAM(PARAM_MODE,            mode,                          &g_WriteCheck_Mode)             },
  { CFGPARAM(PARAM_CH0_ASTRO_MODE,  channelAstroMode[0],           &g_WriteCheck_ChannelAstroMode) },
  { CFGPARAM(PARAM_CH1_ASTRO_MODE,  channelAstroMode[1],           &g_WriteCheck_ChannelAstroMode) },
};

/* =========================================================================
 * Block parameter arrays  (index + offsetof into each block-shaped struct)
 * ========================================================================= */

static BlockParameter_t const m_LocationBlockParams[] = {
  {(uint32_t)PARAM_LATITUDE, offsetof(LocationBlock_t, latitude)},
  {(uint32_t)PARAM_LONGITUDE, offsetof(LocationBlock_t, longitude)},
  {(uint32_t)PARAM_TIMEZONE_OFFSET, offsetof(LocationBlock_t, timezone_offset_minutes)},
  {(uint32_t)PARAM_ASTRO_OFFSET, offsetof(LocationBlock_t, astro_offset_minutes)},
};

static BlockParameter_t const m_ModeBlockParams[] = {
  {(uint32_t)PARAM_MODE, offsetof(ModeBlock_t, mode)},
};

static BlockParameter_t const m_ChannelConfigBlockParams[] = {
  {(uint32_t)PARAM_CH0_ASTRO_MODE,
   offsetof(ChannelConfigBlock_t, channelMode) + (0u * sizeof(ChannelMode_t))},
  {(uint32_t)PARAM_CH1_ASTRO_MODE,
   offsetof(ChannelConfigBlock_t, channelMode) + (1u * sizeof(ChannelMode_t))},
};

/* =========================================================================
 * Block table  (CFGBLOCK macro fills: blockID, initial version, count, params, NULL)
 * ========================================================================= */

/* clang-format off */
static ParameterBlock_t m_Blocks[] =
{
    { CFGBLOCK(BLOCK_LOCATION,       m_LocationBlockParams)      },
    { CFGBLOCK(BLOCK_MODE,           m_ModeBlockParams)          },
    { CFGBLOCK(BLOCK_CHANNEL_CONFIG, m_ChannelConfigBlockParams) },
};
/* clang-format on */

/* =========================================================================
 * Linked-list heads  (built during Init)
 * ========================================================================= */

static Parameter_t *m_pParamHead = NULL;
static ParameterBlock_t *m_pBlockHead = NULL;

/* =========================================================================
 * Private helpers
 * ========================================================================= */

static Parameter_t *FindParam(uint32_t const paramId)
{
  Parameter_t *pParam = m_pParamHead;
  while ((pParam != NULL) && (pParam->index != paramId))
  {
    pParam = pParam->pNextParam;
  }
  return pParam;
}

static ParameterBlock_t *FindBlock(uint32_t const blockId)
{
  ParameterBlock_t *pBlock = m_pBlockHead;
  while ((pBlock != NULL) && (pBlock->blockID != blockId))
  {
    pBlock = pBlock->pNextBlock;
  }
  return pBlock;
}

/* Increment version on every block that contains paramId */
static void IncrementContainingBlockVersions(uint32_t const paramId)
{
  ParameterBlock_t *pBlock = m_pBlockHead;
  while (pBlock != NULL)
  {
    for (uint32_t i = 0u; i < pBlock->numParameters; i++)
    {
      if (pBlock->pParameters[i].index == paramId)
      {
        pBlock->version++;
        break;
      }
    }
    pBlock = pBlock->pNextBlock;
  }
}

/* =========================================================================
 * Public API
 * ========================================================================= */

void ParameterBlock_Init(void)
{
  /* 1. Load defaults into RAM mirror */
  ParameterBlock_DefaultParameters();

  /* 2. Register all parameters (build linked list) */
  for (uint32_t i = 0u; i < PARAM_NUM_ELEM(m_Params); i++)
  {
    ParameterBlock_AddParameter(&m_Params[i]);
  }

  /* 3. Register all blocks */
  for (uint32_t i = 0u; i < PARAM_NUM_ELEM(m_Blocks); i++)
  {
    ParameterBlock_AddBlock(&m_Blocks[i]);
  }

  /* TODO: Load persisted values from HAL_NVM and apply via SetParam */
}

void ParameterBlock_DefaultParameters(void)
{
  memcpy(&m_CfgParams, &m_CfgParamsDefault, sizeof(AstroTimerCfg_t));
}

/* -------------------------------------------------------------------------
 * SetParam  — validate then write one parameter
 * ------------------------------------------------------------------------- */
WriteError_t ParameterBlock_SetParam(AstroTmrParameters_t param, void const *pSource, uint32_t size)
{
  if (pSource == NULL)
  {
    return WRITE_ERROR_NULL_SOURCE;
  }

  Parameter_t *pParam = FindParam((uint32_t)param);
  if (pParam == NULL)
  {
    return WRITE_ERROR_PARAM_NOT_FOUND;
  }
  if (pParam->size != size)
  {
    return WRITE_ERROR_SIZE_MISMATCH;
  }

  /* Phase 1: validate */
  WriteError_t const result = Private_WriteCheck(pParam, pSource, false);
  if (result != WRITE_ERROR_NONE)
  {
    return result;
  }

  /* Phase 2: commit */
  memcpy(pParam->pData, pSource, pParam->size);
  IncrementContainingBlockVersions((uint32_t)param);
  /* TODO: HAL_NVM_Write() to persist */

  return WRITE_ERROR_NONE;
}

/* -------------------------------------------------------------------------
 * GetParam  — read one parameter from the RAM mirror
 * ------------------------------------------------------------------------- */
bool ParameterBlock_GetParam(AstroTmrParameters_t param, void *pDest, uint32_t size)
{
  if (pDest == NULL)
  {
    return false;
  }

  Parameter_t const *pParam = FindParam((uint32_t)param);
  if (pParam == NULL)
  {
    return false;
  }
  if (pParam->size != size)
  {
    return false;
  }

  memcpy(pDest, pParam->pData, pParam->size);
  return true;
}

/* -------------------------------------------------------------------------
 * GetBlock  — copy all parameters in a block into pLocalBlock->pBaseData
 *             and update pLocalBlock->version to match.
 * ------------------------------------------------------------------------- */
void ParameterBlock_GetBlock(Block_t block, LocalBlock_t *pLocalBlock)
{
  if ((pLocalBlock == NULL) || (pLocalBlock->pBaseData == NULL))
  {
    return;
  }

  ParameterBlock_t const *pBlock = FindBlock((uint32_t)block);
  if (pBlock == NULL)
  {
    return;
  }

  for (uint32_t i = 0u; i < pBlock->numParameters; i++)
  {
    BlockParameter_t const *pBP = &pBlock->pParameters[i];
    Parameter_t const *pParam = FindParam(pBP->index);
    if (pParam == NULL)
    {
      continue;
    }
    memcpy((uint8_t *)pLocalBlock->pBaseData + pBP->offset, pParam->pData, pParam->size);
  }

  pLocalBlock->version = pBlock->version;
}

/* -------------------------------------------------------------------------
 * SetBlock  — two-phase commit: validate all, then write all.
 *             Restores the block's version to pLocalBlock->version on success
 *             so the caller can set a known baseline (e.g. after NVM restore).
 * ------------------------------------------------------------------------- */
WriteError_t ParameterBlock_SetBlock(Block_t block, LocalBlock_t const *pLocalBlock)
{
  if ((pLocalBlock == NULL) || (pLocalBlock->pBaseData == NULL))
  {
    return WRITE_ERROR_NULL_SOURCE;
  }

  ParameterBlock_t *pBlock = FindBlock((uint32_t)block);
  if (pBlock == NULL)
  {
    return WRITE_ERROR_PARAM_NOT_FOUND;
  }

  /* Phase 1: validate every parameter in the block */
  for (uint32_t i = 0u; i < pBlock->numParameters; i++)
  {
    BlockParameter_t const *pBP = &pBlock->pParameters[i];
    Parameter_t const *pParam = FindParam(pBP->index);
    if (pParam == NULL)
    {
      return WRITE_ERROR_PARAM_NOT_FOUND;
    }

    void const *pSrc = (uint8_t const *)pLocalBlock->pBaseData + pBP->offset;
    WriteError_t const result = Private_WriteCheck(pParam, pSrc, false);
    if (result != WRITE_ERROR_NONE)
    {
      return result;
    }
  }

  /* Phase 2: commit all parameters */
  for (uint32_t i = 0u; i < pBlock->numParameters; i++)
  {
    BlockParameter_t const *pBP = &pBlock->pParameters[i];
    Parameter_t const *pParam = FindParam(pBP->index);
    void const *pSrc = (uint8_t const *)pLocalBlock->pBaseData + pBP->offset;
    memcpy(pParam->pData, pSrc, pParam->size);
  }

  /* Restore block version to caller's baseline (allows NVM reload to sync state) */
  pBlock->version = pLocalBlock->version;
  /* TODO: HAL_NVM_Write() block to persist */

  return WRITE_ERROR_NONE;
}

/* -------------------------------------------------------------------------
 * UpdateBlock  — returns true and refreshes pLocalBlock only when the block
 *                version has changed since the last call.
 * ------------------------------------------------------------------------- */
bool ParameterBlock_UpdateBlock(Block_t block, LocalBlock_t *pLocalBlock)
{
  if (pLocalBlock == NULL)
  {
    return false;
  }

  ParameterBlock_t const *pBlock = FindBlock((uint32_t)block);
  if (pBlock == NULL)
  {
    return false;
  }

  if (pBlock->version == pLocalBlock->version)
  {
    return false;
  }

  /* Version mismatch — refresh the caller's buffer */
  ParameterBlock_GetBlock(block, pLocalBlock);
  return true;
}

/* -------------------------------------------------------------------------
 * AddParameter  — append to the linked list (for runtime extensibility)
 * ------------------------------------------------------------------------- */
void ParameterBlock_AddParameter(Parameter_t *pParam)
{
  if (pParam == NULL)
  {
    return;
  }

  Parameter_t **ppNode = &m_pParamHead;
  while (*ppNode != NULL)
  {
    ppNode = &(*ppNode)->pNextParam;
  }
  *ppNode = pParam;
  pParam->pNextParam = NULL;
}

/* -------------------------------------------------------------------------
 * AddBlock  — append to the linked list (for runtime extensibility)
 * ------------------------------------------------------------------------- */
void ParameterBlock_AddBlock(ParameterBlock_t *pBlock)
{
  if (pBlock == NULL)
  {
    return;
  }

  ParameterBlock_t **ppNode = &m_pBlockHead;
  while (*ppNode != NULL)
  {
    ppNode = &(*ppNode)->pNextBlock;
  }
  *ppNode = pBlock;
  pBlock->pNextBlock = NULL;
}
