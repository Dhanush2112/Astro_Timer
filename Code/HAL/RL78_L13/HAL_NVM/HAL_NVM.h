/* HAL_NVM.h
 * Hardware abstraction for non-volatile memory (flash / data flash / EEPROM).
 */

#ifndef HAL_NVM_H
#define HAL_NVM_H

#include "common.h"

void HAL_NVM_Init(void);
void HAL_NVM_Read(uint32_t address, void *data, uint32_t length);
void HAL_NVM_Write(uint32_t address, const void *data, uint32_t length);

#endif /* HAL_NVM_H */
