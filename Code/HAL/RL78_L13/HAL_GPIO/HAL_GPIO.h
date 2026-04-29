/* HAL_GPIO.h
 * Hardware abstraction for digital output channels (relays).
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "common.h"

void HAL_GPIO_Init(void);
void HAL_GPIO_SetChannel(uint8_t channel, bool state);
bool HAL_GPIO_GetChannel(uint8_t channel);

#endif /* HAL_GPIO_H */
