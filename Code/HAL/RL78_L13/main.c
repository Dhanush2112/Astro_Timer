/* main.c
 * Platform entry point for RL78/L13 (R5F10WMG).
 * Initialises all hardware peripherals, then all application modules,
 * then runs the bare-metal main loop.
 */

#include "App/Astro/Astro.h"
#include "App/CfgDataStore/ParameterBlock.h"
#include "App/DateTime/DateTime.h"
#include "App/LUI/LUI.h"
#include "App/Scheduler.h"
#include "HAL_Display/HAL_Display.h"
#include "HAL_GPIO/HAL_GPIO.h"
#include "HAL_NVM/HAL_NVM.h"
#include "HAL_RTC/HAL_RTC.h"

/* Set to true by the RTC 1-minute periodic interrupt handler */
static volatile bool g_OneMinuteFlag = false;

/* Called from the RL78/L13 RTC periodic interrupt (configured for 1-minute interval) */
void HAL_RTC_MinuteCallback(void)
{
  g_OneMinuteFlag = true;
}

int main(void)
{
  /* --- Hardware initialisation --- */
  HAL_NVM_Init();
  HAL_RTC_Init();
  HAL_GPIO_Init();
  HAL_Display_Init();

  /* --- Application module initialisation --- */
  ParameterBlock_Init();
  DateTime_Init();
  Astro_Init();
  Scheduler_Init();
  LUI_Init();

  /* --- Main loop --- */
  while (1)
  {
    if (g_OneMinuteFlag)
    {
      Scheduler_RunMinuteTick();
      g_OneMinuteFlag = false;
    }

    LUI_Poll();
  }
}
