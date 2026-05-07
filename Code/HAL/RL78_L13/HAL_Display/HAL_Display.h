/* HAL_Display.h
 * Hardware abstraction for status LEDs and display indicators.
 */

#ifndef HAL_DISPLAY_H
#define HAL_DISPLAY_H

#include "common.h"

void HAL_Display_Init(void);
void HAL_Display_SetChannelLed(uint8_t channel, bool on);
void HAL_Display_SetModeLed(uint8_t mode, bool on);

#endif /* HAL_DISPLAY_H */
