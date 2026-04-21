/* CfgDataStore_WriteChecks.c
 * Implements:
 *   - Range / validity check functions for each configurable parameter.
 *   - Private_WriteCheck() — the private dispatcher called by
 *     CfgDataStore_ParameterBlock.c before every write.
 */

#include "WriteChecks.h"

#include "Private.h"

/* =========================================================================
 * Static range-check helpers
 * ========================================================================= */

static WriteError_t FloatRangeCheck(Parameter_t const * const pParam,
                                    void const * const pSource,
                                    float32_t lower,
                                    float32_t upper)
{
  (void)pParam;
  if (pSource == NULL)
  {
    return WRITE_ERROR_NULL_SOURCE;
  }
  float32_t val;
  memcpy(&val, pSource, sizeof(float32_t));
  if ((val < lower) || (val > upper))
  {
    return WRITE_ERROR_INVALID;
  }
  return WRITE_ERROR_NONE;
}

static WriteError_t Int16RangeCheck(Parameter_t const * const pParam,
                                    void const * const pSource,
                                    int16_t lower,
                                    int16_t upper)
{
  (void)pParam;
  if (pSource == NULL)
  {
    return WRITE_ERROR_NULL_SOURCE;
  }
  int16_t val;
  memcpy(&val, pSource, sizeof(int16_t));
  if ((val < lower) || (val > upper))
  {
    return WRITE_ERROR_INVALID;
  }
  return WRITE_ERROR_NONE;
}

/* =========================================================================
 * Per-parameter check functions
 * ========================================================================= */

static WriteError_t CheckLatitude(Parameter_t const * const pParam, void const * const pSource)
{
  return FloatRangeCheck(pParam, pSource, -90.0f, 90.0f);
}

static WriteError_t CheckLongitude(Parameter_t const * const pParam, void const * const pSource)
{
  return FloatRangeCheck(pParam, pSource, -180.0f, 180.0f);
}

static WriteError_t CheckTimezoneOffset(Parameter_t const * const pParam,
                                        void const * const pSource)
{
  /* UTC-12 (−720 min) to UTC+14 (+840 min) */
  return Int16RangeCheck(pParam, pSource, -720, 840);
}

static WriteError_t CheckAstroOffset(Parameter_t const * const pParam, void const * const pSource)
{
  return Int16RangeCheck(pParam, pSource, -120, 120);
}

static WriteError_t CheckMode(Parameter_t const * const pParam, void const * const pSource)
{
  (void)pParam;
  if (pSource == NULL)
  {
    return WRITE_ERROR_NULL_SOURCE;
  }
  OperatingMode_t val;
  memcpy(&val, pSource, sizeof(OperatingMode_t));
  if ((val != MODE_AUTO) && (val != MODE_RANDOM) && (val != MODE_HOLIDAY) && (val != MODE_MANUAL))
  {
    return WRITE_ERROR_INVALID;
  }
  return WRITE_ERROR_NONE;
}

static WriteError_t CheckChannelAstroMode(Parameter_t const * const pParam,
                                          void const * const pSource)
{
  (void)pParam;
  if (pSource == NULL)
  {
    return WRITE_ERROR_NULL_SOURCE;
  }
  ChannelMode_t val;
  memcpy(&val, pSource, sizeof(ChannelMode_t));
  if ((val != CHANNEL_MODE_DISABLED) && (val != CHANNEL_MODE_SUNSET_ON)
      && (val != CHANNEL_MODE_SUNRISE_ON))
  {
    return WRITE_ERROR_INVALID;
  }
  return WRITE_ERROR_NONE;
}

/* =========================================================================
 * Public write-check instances
 * ========================================================================= */

WriteCheck_t const g_WriteCheck_Latitude = {CheckLatitude};
WriteCheck_t const g_WriteCheck_Longitude = {CheckLongitude};
WriteCheck_t const g_WriteCheck_TimezoneOffset = {CheckTimezoneOffset};
WriteCheck_t const g_WriteCheck_AstroOffset = {CheckAstroOffset};
WriteCheck_t const g_WriteCheck_Mode = {CheckMode};
WriteCheck_t const g_WriteCheck_ChannelAstroMode = {CheckChannelAstroMode};

/* =========================================================================
 * Private write-check dispatcher  (declared in Private.h)
 *
 * Called by CfgDataStore_ParameterBlock.c for every write attempt.
 * Only validates; the actual memcpy is performed by the caller so that
 * the parameter engine can implement a two-phase commit across a block.
 * ========================================================================= */

WriteError_t Private_WriteCheck(Parameter_t const * const pParam,
                                void const * const pSource,
                                bool updateParameter)
{
  (void)updateParameter; /* No value-adjustment checks in this project */

  if ((pParam == NULL) || (pSource == NULL))
  {
    return WRITE_ERROR_NULL_SOURCE;
  }

  if ((pParam->pWriteCheck == NULL) || (pParam->pWriteCheck->pFn == NULL))
  {
    return WRITE_ERROR_NONE; /* No check registered — always accept */
  }

  return pParam->pWriteCheck->pFn(pParam, pSource);
}
