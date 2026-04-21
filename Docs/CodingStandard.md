# Coding Standard

## 1. Purpose
This document defines a reusable coding standard applicable across multiple projects. It aims to improve code readability, safety, portability, maintainability, and review efficiency.

## 2. Scope
This standard applies to all production source code unless explicitly exempted. Project-specific deviations must be documented.

## 3. General Principles
- Write clear, readable, and maintainable code
- Prefer simplicity over cleverness
- All code must be reviewed
- Undefined, implementation-defined, and unspecified behavior shall be avoided

## 4. Languages and Standards
### 4.1 C / C++ (if applicable)
- C: ISO C99 or later as defined by the project
- C++: C++17 or later as defined by the project
- Follow MISRA C / MISRA C++ where safety or reliability is required

### 4.2 Other Languages
- Follow the most recent stable language standard approved for the project
- Follow well-known community style guides unless overridden

## 5. Tooling and Static Analysis
- Enable compiler warnings at the highest reasonable level
- Treat warnings as errors where practical
- Use static analysis tools (e.g., clang-tidy, PC-lint, cppcheck)
- All suppressions must be justified and reviewed

## 6. Code Formatting
- Automated formatting tools should be used where available (e.g., clang-format)
- Use spaces over tabs unless project specifies otherwise
- Maximum line length: 100 characters (default)

## 7. Naming Conventions
### 7.1 General
- Names must be descriptive and unambiguous
- Avoid abbreviations unless they are well-known

### 7.2 Files
- Use PascalCase consistently per project
- Source and header file names must match

### 7.3 Variables
- Use PascalCase for the portion of file-global variable names after the `g_` prefix (e.g., `g_CurrentMode`, `g_ProgramCount`).
- All file-global variables must be declared `static` (internal linkage). Do not use non-static globals; avoid `extern` globals where possible.
- Use snake_case for local variables.
- Globals must be clearly identifiable by the `g_` prefix.
- Avoid single-letter names except for loop indices

### 7.4 Functions
- Use verbs or verb phrases
- Avoid overly long function names, but prioritize clarity

### 7.5 Types
- Types use PascalCase
- Type aliases end with _t where common (C projects)

## 8. Comments and Documentation
- Code should be self-explanatory first
- Comments explain *why*, not *what*
- Document all public APIs
- Remove commented-out code before commit

## 9. Functions and Complexity
- Functions must do one logical task
- Target cyclomatic complexity ≤ 15
- Prefer early returns only when they improve clarity

## 10. Error Handling
- Always check return values unless explicitly safe
- Never ignore errors silently
- Propagate or handle errors appropriately

## 11. Memory and Resource Management
- Avoid dynamic allocation in constrained or safety-critical code
- Every resource acquisition must have a clear release point
- Prefer RAII (C++) or explicit ownership rules (C)

## 12. Macros and Preprocessor Use (C/C++)
- Avoid macros when functions or const variables are possible
- Macros must be fully parenthesized
- Macros must be documented

## 13. Global State
- Minimize use of global variables
- Globals must be justified and documented
- Prefer encapsulation within modules

## 14. Concurrency and Interrupt Safety
- Shared data must be protected
- Document assumptions about threading or interrupts
- Avoid hidden race conditions

## 15. Security and Safety
- Validate all external inputs
- Avoid buffer overflows, integer overflows, and undefined behavior
- Follow secure coding guidelines relevant to the platform

## 16. File Structure
- Headers must be self-contained
- Use include guards or #pragma once
- Avoid unnecessary includes

## 17. Version Control
- Each commit should be logically complete
- Write clear commit messages
- Do not commit generated or temporary files

## 18. Deviations
- Deviations from this standard must be:
  - Documented
  - Justified
  - Reviewed and approved

## 19. Review Checklist (Summary)
- Correctness
- Readability
- Compliance with this standard
- Test coverage where applicable

## 20. Revision History
| Date | Version | Description |
|------|---------|-------------|
| 2026-04-21 | 1.0 | Initial generic coding standard |