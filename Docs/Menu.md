# Menu Module

Purpose
- User interface layer for configuring programs, mode selection, offsets, and manual control.

Public API (from `Code/App/Menu/Menu.h`)
- `void Menu_Init(void);`

Behavior & notes
- The Menu module should present configuration screens and accept user input.
- UI rendering and input handling are platform-specific (LCD, buttons, encoder) and may rely on HAL.

Implementation TODOs
- Define UI flow and screen mockups.
- Implement input debounce/long-press behavior.
- Add UI tests or a headless simulation harness.

Examples
- `Menu_Init();`