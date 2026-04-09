/* Modes.c
 * Modes implementation (placeholder)
 */

#include "Modes.h"

static ModeType current_mode = MODE_AUTO;

void Modes_Init(void) { current_mode = MODE_AUTO; }
ModeType Modes_GetCurrent(void) { return current_mode; }