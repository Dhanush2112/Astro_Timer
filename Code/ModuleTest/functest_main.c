/* functest_main.c
 * Shared entry point for all per-module test executables.
 * Calls Unity''s UnityMain which runs the test group registered
 * in RunAllTests() of the accompanying test file.
 */

#include <unity_fixture.h>

extern void RunAllTests(void);

int main(int argc, const char *argv[])
{
  return UnityMain(argc, argv, RunAllTests);
}
