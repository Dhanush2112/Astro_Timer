# Channels Module

Purpose
- Control and report the state of physical output channels (two channels by default).

Public API (from `Code/App/Channels/Channels.h`)
- `void Channels_Init(void);`
- `void Channels_SetState(uint8_t channel, bool on);`
- `bool Channels_GetState(uint8_t channel);`

Behavior & notes
- `channel` is 0-based and must be < `CHANNEL_COUNT`.
- Hardware mapping (GPIO, relay drivers) should be implemented in HAL and called by Channels module.
- Channels should debounce or serialize hardware access if required by the platform.

Implementation TODOs
- Wire channels to HAL functions for actual hardware control.
- Add optional callbacks/events when channel state changes for UI/telemetry.

Examples
- `Channels_Init();`
- `Channels_SetState(0, true); // turn channel 0 ON`