# Programs Module

Purpose
- Store and manage scheduled programs (daily or yearly) that trigger channel state changes.

Public API (from `Code/App/Programs/Programs.h`)
- `void Programs_Init(void);`
- `int Programs_Add(ProgramType type, const char *spec);` // returns id or -1 on failure
- `void Programs_Remove(int id);`

Types
- `typedef enum { PROGRAM_DAILY, PROGRAM_YEARLY } ProgramType;`

Behavior & notes
- Program evaluation logic (matching current datetime to program rules) may be implemented in Programs or delegated to Scheduler.
- Store program definitions in persistent storage (via `Config`/`Data`) so they survive power cycles.
- Max programs: `MAX_PROGRAMS` (default 100).

Implementation TODOs
- Define a concise `spec` syntax for program definitions or introduce an explicit struct API.
- Implement persistence and validation of program data.
- Add unit tests for parsing and evaluation of daily/yearly rules.

Examples
- `Programs_Add(PROGRAM_DAILY, "Mon-Fri 07:00 ON");`