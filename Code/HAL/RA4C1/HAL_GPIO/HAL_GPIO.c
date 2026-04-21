/* HAL_GPIO.c
 * Hardware abstraction for digital output channels (relays) — RA4C1 implementation.
 */

#include "HAL_GPIO.h"

void HAL_GPIO_Init(void)
{
  /* TODO: configure relay output pins on RA4C1 */
}

void HAL_GPIO_SetChannel(uint8_t channel, bool state)
{
  (void)channel;
  (void)state;
  /* TODO: write pin state via RA4C1 GPIO registers */
}

bool HAL_GPIO_GetChannel(uint8_t channel)
{
  (void)channel;
  /* TODO: read pin state from RA4C1 GPIO registers */
  return false;
}
