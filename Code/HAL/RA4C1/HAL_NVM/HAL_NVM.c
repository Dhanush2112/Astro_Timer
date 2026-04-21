/* HAL_NVM.c
 * Hardware abstraction for non-volatile memory — RA4C1 implementation.
 * Uses RA4C1 data flash for persistent configuration storage.
 */

#include "HAL_NVM.h"

void HAL_NVM_Init(void)
{
  /* TODO: initialise RA4C1 data flash controller */
}

void HAL_NVM_Read(uint32_t address, void *data, uint32_t length)
{
  (void)address;
  (void)data;
  (void)length;
  /* TODO: read from RA4C1 data flash */
}

void HAL_NVM_Write(uint32_t address, const void *data, uint32_t length)
{
  (void)address;
  (void)data;
  (void)length;
  /* TODO: erase sector and write to RA4C1 data flash */
}
