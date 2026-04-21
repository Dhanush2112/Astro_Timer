/* ParameterBlock.h
 * Public API for the CfgDataStore parameter-block engine.
 *
 * Usage pattern
 * -------------
 * 1.  Call ParameterBlock_Init() once at startup — loads defaults, registers all
 *     parameters and blocks, then restores persisted values from HAL_NVM.
 *
 * 2.  GetParam / SetParam — individual parameter access.
 *
 * 3.  GetBlock / SetBlock — atomic access to a group of related parameters
 *     using the corresponding block-shaped struct (LocationBlock_t etc.).
 *
 * 4.  UpdateBlock — call periodically; returns true and fills the caller's
 *     LocalBlock_t only when any parameter in the block has changed since the
 *     last call.  Enables efficient change-detection without polling.
 *
 * Helper macros (used inside CfgDataStore_ParameterBlock.c only)
 * -------------------------------------------------------------
 * CFGPARAM(paramId, field, writeCheck)
 *   Expands to the initialiser fields of a Parameter_t for the given field
 *   inside the static RAM-mirror struct m_CfgParams.
 *
 * CFGBLOCK(blockId, paramsArray)
 *   Expands to the initialiser fields of a ParameterBlock_t.
 *   The initial version 0xEFFFFFFFU ensures UpdateBlock fires on the first call.
 */

#ifndef PARAMETERBLOCK_H
#define PARAMETERBLOCK_H

#include "ParameterBlockTypes.h"

/* ---- Helper macros (only meaningful inside CfgDataStore_ParameterBlock.c) ---- */

/* m_CfgParams is the file-scope RAM-mirror struct defined in ParameterBlock.c  */
#define CFGPARAM(paramId, field, writeCheck)                                                       \
  (uint32_t)(paramId), (uint32_t)sizeof(m_CfgParams.field), (void *)&(m_CfgParams.field), NULL,    \
    (writeCheck)

#define CFGBLOCK(blockId, paramsArray)                                                             \
  (uint32_t)(blockId), 0xEFFFFFFFU, (uint32_t)PARAM_NUM_ELEM(paramsArray), &(paramsArray)[0], NULL

#define NO_WRITE_CHECK NULL

/* ---- Public API ---- */

void ParameterBlock_Init(void);
void ParameterBlock_DefaultParameters(void);

WriteError_t ParameterBlock_SetParam(AstroTmrParameters_t param, void const *pSource, uint32_t size);

bool ParameterBlock_GetParam(AstroTmrParameters_t param, void *pDest, uint32_t size);

void ParameterBlock_GetBlock(Block_t block, LocalBlock_t *pLocalBlock);

WriteError_t ParameterBlock_SetBlock(Block_t block, LocalBlock_t const *pLocalBlock);

bool ParameterBlock_UpdateBlock(Block_t block, LocalBlock_t *pLocalBlock);

/* Extensibility: register additional parameters/blocks at runtime */
void ParameterBlock_AddParameter(Parameter_t *pParam);
void ParameterBlock_AddBlock(ParameterBlock_t *pBlock);

#endif /* PARAMETERBLOCK_H */
