/* HAL_NVM.c
 * Hardware abstraction for non-volatile memory — RL78/L13 implementation.
 * Uses RL78/L13 data flash for persistent configuration storage.
 */

#include "HAL_NVM.h"

void HAL_NVM_Init(void)
{
  /* TODO: initialise RL78/L13 data flash controller */
}

void HAL_NVM_Read(uint32_t address, void *data, uint32_t length)
{
  (void)address;
  (void)data;
  (void)length;
  /* TODO: read from RL78/L13 data flash */
}

void HAL_NVM_Write(uint32_t address, const void *data, uint32_t length)
{
  (void)address;
  (void)data;
  (void)length;
  /* TODO: erase sector and write to RL78/L13 data flash */
}
