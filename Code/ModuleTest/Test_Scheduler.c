/* Test_Scheduler.c
 * Unit tests for the Scheduler module.
 */

#include "Scheduler.h"
#include "Test_Common.h"

TEST_GROUP(Scheduler);

TEST_SETUP(Scheduler)
{
  Scheduler_Init();
}

TEST_TEAR_DOWN(Scheduler)
{
}

TEST(Scheduler, InitAndTick)
{
  /* Smoke test: RunMinuteTick must not crash with default (placeholder) state */
  Scheduler_RunMinuteTick();
  TEST_PASS();
}

void RunAllTests(void)
{
  RUN_TEST_CASE(Scheduler, InitAndTick);
}
