# Data Module

Purpose
- Generic data helpers and storage utilities used by other modules (Programs, Config).

Public API (from `Code/App/Data/Data.h`)
- `void Data_Init(void);`

Behavior & notes
- Acts as a small abstraction layer for data layout and serialization helpers.
- May provide helper functions for reading/writing structured data to persistent storage.

Implementation TODOs
- Add serialization helpers for program lists, holiday ranges, and other structured config items.
- Provide data migration strategies for future format changes.

Examples
- `Data_Init();`