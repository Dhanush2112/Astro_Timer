/* Channels.c
 * Placeholder implementation for Channels module
 */

#include "Channels.h"

static bool channel_states[CHANNEL_COUNT];

void Channels_Init(void) {
    for (int i = 0; i < CHANNEL_COUNT; i++) channel_states[i] = false;
}

void Channels_SetState(uint8_t channel, bool on) {
    if (channel < CHANNEL_COUNT) channel_states[channel] = on;
}

bool Channels_GetState(uint8_t channel) {
    if (channel < CHANNEL_COUNT) return channel_states[channel];
    return false;
}
