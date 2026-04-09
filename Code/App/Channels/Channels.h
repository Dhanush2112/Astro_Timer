/* Channels.h
 * Placeholder for Channels module
 * Implements two physical channels and their control API
 * See Docs/Design_Reference.md for requirements
 */

#ifndef CHANNELS_H
#define CHANNELS_H

#include <stdint.h>
#include <stdbool.h>

#define CHANNEL_COUNT 2

void Channels_Init(void);
void Channels_SetState(uint8_t channel, bool on);
bool Channels_GetState(uint8_t channel);

#endif /* CHANNELS_H */
