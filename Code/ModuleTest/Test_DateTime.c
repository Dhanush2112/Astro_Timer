/* Test_DateTime.c
 * Unit tests for the DateTime module.
 */

#include <unity_fixture.h>

#include "DateTime/DateTime.h"

TEST_GROUP(DateTime);

TEST_SETUP(DateTime)
{
  DateTime_Init();
}

TEST_TEAR_DOWN(DateTime)
{
}

TEST(DateTime, InitAndGet)
{
  int16_t year;
  uint8_t month, day, hour, minute, second;
  DateTime_Get(&year, &month, &day, &hour, &minute, &second);
  /* Placeholder returns 2026-01-01 00:00:00 */
  TEST_ASSERT_EQUAL_INT16(2026, year);
  TEST_ASSERT_EQUAL_UINT8(1, month);
  TEST_ASSERT_EQUAL_UINT8(1, day);
}

void RunAllTests(void)
{
  RUN_TEST_CASE(DateTime, InitAndGet);
}
