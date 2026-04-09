/* Astro.h
 * Astronomical calculations and offsets
 */

#ifndef ASTRO_H
#define ASTRO_H

#include <stdint.h>

void Astro_Init(void);
void Astro_UpdateForDate(int year, int month, int day);
// Returns sunrise and sunset minutes from midnight (local time)
int Astro_GetSunriseMinutes(void);
int Astro_GetSunsetMinutes(void);

#endif /* ASTRO_H */