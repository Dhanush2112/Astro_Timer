/* HAL_GPIO.c
 * Hardware abstraction for digital output channels (relays) — RL78/L13 implementation.
 */

#include "HAL_GPIO.h"

void HAL_GPIO_Init(void)
{
  /* TODO: configure relay output pins on RL78/L13 */
}

void HAL_GPIO_SetChannel(uint8_t channel, bool state)
{
  (void)channel;
  (void)state;
  /* TODO: write pin state via RL78/L13 port registers */
}

bool HAL_GPIO_GetChannel(uint8_t channel)
{
  (void)channel;
  /* TODO: read pin state from RL78/L13 port registers */
  return false;
}
