/* Private.h
 * Internal header — included only by CfgDataStore_ParameterBlock.c
 * and CfgDataStore_WriteChecks.c.  NOT for external consumers.
 *
 * Declares the private write-check dispatcher that bridges the parameter
 * engine (ParameterBlock) with the validation layer (WriteChecks).
 */

#ifndef PRIVATE_H
#define PRIVATE_H

#include "ParameterBlockTypes.h"

/* Implemented in CfgDataStore_WriteChecks.c; called from CfgDataStore_ParameterBlock.c.
 *
 * Runs the write-check callback registered with pParam.
 * If updateParameter is true AND the check passes, the new value is NOT copied here —
 * the caller (ParameterBlock.c) performs the memcpy so it can do the two-phase commit.
 *
 * Returns WRITE_ERROR_NONE if the value is acceptable. */
extern WriteError_t Private_WriteCheck(Parameter_t const * const pParam,
                                       void const * const pSource,
                                       bool updateParameter);

#endif /* PRIVATE_H */
