/* Astro.h
 * Astronomical calculations and offsets
 */

#ifndef ASTRO_H
#define ASTRO_H

#include "common.h"

void Astro_Init(void);
void Astro_UpdateForDate(int16_t year, uint8_t month, uint8_t day);
// Returns sunrise and sunset minutes from midnight (local time)
int16_t Astro_GetSunriseMinutes(void);
int16_t Astro_GetSunsetMinutes(void);

#endif /* ASTRO_H */
