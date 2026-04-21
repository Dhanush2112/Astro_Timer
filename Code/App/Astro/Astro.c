/* Astro.c
 * Astronomical calculations (placeholder)
 */

#include "Astro.h"

static int16_t g_SunriseMinutes = 6 * 60;  // 06:00
static int16_t g_SunsetMinutes = 18 * 60;  // 18:00

void Astro_Init(void)
{
  // placeholder initialization
}

void Astro_UpdateForDate(int16_t year, uint8_t month, uint8_t day)
{
  (void)year;
  (void)month;
  (void)day;
  // placeholder: keep defaults
}

int16_t Astro_GetSunriseMinutes(void)
{
  return g_SunriseMinutes;
}

int16_t Astro_GetSunsetMinutes(void)
{
  return g_SunsetMinutes;
}
