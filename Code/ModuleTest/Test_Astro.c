/* Test_Astro.c
 * Unit tests for the Astro module (sunrise/sunset calculation).
 */

#include <unity_fixture.h>

#include "Astro/Astro.h"

TEST_GROUP(Astro);

TEST_SETUP(Astro)
{
  Astro_Init();
}

TEST_TEAR_DOWN(Astro)
{
}

TEST(Astro, DefaultValues)
{
  Astro_UpdateForDate(2026, 4, 9);
  int16_t sr = Astro_GetSunriseMinutes();
  int16_t ss = Astro_GetSunsetMinutes();
  /* Placeholder implementation returns 360 (06:00) and 1080 (18:00) */
  TEST_ASSERT_EQUAL_INT16(360, sr);
  TEST_ASSERT_EQUAL_INT16(1080, ss);
  TEST_ASSERT_TRUE(sr < ss);
}

void RunAllTests(void)
{
  RUN_TEST_CASE(Astro, DefaultValues);
}
