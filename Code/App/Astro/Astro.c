/* Astro.c
 * Astronomical calculations (placeholder)
 */

#include "Astro.h"

static int sunrise_minutes = 6*60; // 06:00
static int sunset_minutes = 18*60; // 18:00

void Astro_Init(void) {
    // placeholder initialization
}

void Astro_UpdateForDate(int year, int month, int day) {
    (void)year; (void)month; (void)day;
    // placeholder: keep defaults
}

int Astro_GetSunriseMinutes(void) { return sunrise_minutes; }
int Astro_GetSunsetMinutes(void) { return sunset_minutes; }