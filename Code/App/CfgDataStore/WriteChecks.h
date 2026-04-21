/* WriteChecks.h
 * Public write-check instances for each configurable parameter.
 * These are passed into the parameter registration table in
 * CfgDataStore_ParameterBlock.c and called before any write to the RAM mirror.
 */

#ifndef WRITECHECKS_H
#define WRITECHECKS_H

#include "ParameterBlockTypes.h"

extern WriteCheck_t const g_WriteCheck_Latitude;         /* float32_t [-90.0,  +90.0]  degrees */
extern WriteCheck_t const g_WriteCheck_Longitude;        /* float32_t [-180.0, +180.0] degrees */
extern WriteCheck_t const g_WriteCheck_TimezoneOffset;   /* int16_t [-720, +840]    minutes */
extern WriteCheck_t const g_WriteCheck_AstroOffset;      /* int16_t [-120, +120]    minutes */
extern WriteCheck_t const g_WriteCheck_Mode;             /* valid ModeType enum value        */
extern WriteCheck_t const g_WriteCheck_ChannelAstroMode; /* valid ChannelAstroMode value     */

#endif /* WRITECHECKS_H */
