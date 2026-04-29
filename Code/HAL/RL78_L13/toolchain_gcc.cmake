# Code/HAL/RL78_L13/toolchain_gcc.cmake
# GCC rl78-elf toolchain for Renesas RL78/L13 (R5F10WMG).
#
# This file is platform-specific. A different target platform requires
# a separate toolchain file.
#
# By default the toolchain executables are resolved via the system PATH,
# so no per-developer configuration is needed as long as the GCC RL78
# toolchain is installed and its bin directory is on PATH.
#
# If the toolchain bin directory is NOT on PATH, set the environment
# variable RL78_GCC_TOOLCHAIN_DIR to the bin directory before invoking
# CMake, e.g.:
#   $env:RL78_GCC_TOOLCHAIN_DIR = "C:/ProgramData/GCC for Renesas RL78 4.9.2.202201-GNURL78-ELF/rl78-elf/rl78-elf/bin"

# ---------------------------------------------------------------------------
# System description — bare-metal RL78 target
# ---------------------------------------------------------------------------
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR RL78)

# ---------------------------------------------------------------------------
# Toolchain executables
# Resolved via system PATH by default.
# Set RL78_GCC_TOOLCHAIN_DIR to override (useful in CI or non-standard installs).
# ---------------------------------------------------------------------------
if(DEFINED ENV{RL78_GCC_TOOLCHAIN_DIR})
    set(_RL78_PREFIX "$ENV{RL78_GCC_TOOLCHAIN_DIR}/")
else()
    set(_RL78_PREFIX "")
endif()

set(CMAKE_C_COMPILER   "${_RL78_PREFIX}rl78-elf-gcc.exe"     CACHE FILEPATH "RL78 GCC C compiler")
set(CMAKE_ASM_COMPILER "${_RL78_PREFIX}rl78-elf-gcc.exe"     CACHE FILEPATH "RL78 GCC assembler")
set(CMAKE_AR           "${_RL78_PREFIX}rl78-elf-ar.exe"      CACHE FILEPATH "RL78 GCC archiver")
set(CMAKE_LINKER       "${_RL78_PREFIX}rl78-elf-gcc.exe"     CACHE FILEPATH "RL78 GCC linker (via compiler driver)")
set(CMAKE_OBJCOPY      "${_RL78_PREFIX}rl78-elf-objcopy.exe" CACHE FILEPATH "RL78 GCC objcopy")

# Prevent CMake from running link-based compiler feature tests on the host.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ---------------------------------------------------------------------------
# CPU flags — Renesas RL78/L13 (R5F10WMG)
# Use -mcpu=g13 for the standard RL78 core with hardware multiply unit.
# ---------------------------------------------------------------------------
set(_CPU_FLAGS "-mcpu=g13")

# ---------------------------------------------------------------------------
# Compiler flags (INIT values are written to cache on first configure only)
# ---------------------------------------------------------------------------
set(CMAKE_C_FLAGS_INIT
    "${_CPU_FLAGS} -std=c99 -ffreestanding -ffunction-sections -fdata-sections -Wall"
    CACHE INTERNAL "")
set(CMAKE_ASM_FLAGS_INIT
    "${_CPU_FLAGS} -x assembler-with-cpp"
    CACHE INTERNAL "")

# Debug — minimal optimisation, full DWARF debug information
set(CMAKE_C_FLAGS_DEBUG   "-Og -g3"
    CACHE STRING "GCC C flags for Debug build")
set(CMAKE_ASM_FLAGS_DEBUG "-g3"
    CACHE STRING "GCC ASM flags for Debug build")

# Release — optimise for size, no debug information
set(CMAKE_C_FLAGS_RELEASE   "-Os -DNDEBUG"
    CACHE STRING "GCC C flags for Release build")
set(CMAKE_ASM_FLAGS_RELEASE ""
    CACHE STRING "GCC ASM flags for Release build")

# ---------------------------------------------------------------------------
# Executable output extension
# ---------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf" CACHE INTERNAL "")

# ---------------------------------------------------------------------------
# Sysroot — keep CMake from searching host system paths for libraries
# ---------------------------------------------------------------------------
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
