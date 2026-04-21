# Code/HAL/RA4C1/toolchain_iar.cmake
# IAR Embedded Workbench toolchain for Renesas RA4C1 (Arm Cortex-M33).
#
# This file is platform-specific. A different target platform requires
# a separate toolchain file (e.g. toolchain_iar_RA6M4.cmake).
#
# To override the default installation path set the environment variable
# IAR_ARM_TOOLCHAIN_DIR before invoking CMake, e.g.:
#   $env:IAR_ARM_TOOLCHAIN_DIR = "C:/Program Files/IAR Systems/EW 9.60/arm"

# ---------------------------------------------------------------------------
# System description — bare-metal ARM target
# ---------------------------------------------------------------------------
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# ---------------------------------------------------------------------------
# IAR toolchain location
# ---------------------------------------------------------------------------
if(DEFINED ENV{IAR_ARM_TOOLCHAIN_DIR})
    set(IAR_TOOLCHAIN_PATH "$ENV{IAR_ARM_TOOLCHAIN_DIR}"
        CACHE PATH "IAR ARM toolchain root directory")
else()
    set(IAR_TOOLCHAIN_PATH "C:/Program Files/IAR Systems/Embedded Workbench 9.2/arm"
        CACHE PATH "IAR ARM toolchain root directory")
endif()

set(CMAKE_C_COMPILER   "${IAR_TOOLCHAIN_PATH}/bin/iccarm.exe"   CACHE FILEPATH "IAR C compiler")
set(CMAKE_ASM_COMPILER "${IAR_TOOLCHAIN_PATH}/bin/iasmarm.exe"  CACHE FILEPATH "IAR assembler")
set(CMAKE_AR           "${IAR_TOOLCHAIN_PATH}/bin/iarchive.exe" CACHE FILEPATH "IAR archiver")
set(CMAKE_LINKER       "${IAR_TOOLCHAIN_PATH}/bin/ilinkarm.exe" CACHE FILEPATH "IAR linker")

# Prevent CMake from running link-based compiler feature tests on the host.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ---------------------------------------------------------------------------
# CPU / FPU flags — Renesas RA4C1 uses Arm Cortex-M33 with FPv5-SP
# ---------------------------------------------------------------------------
set(_CPU_FLAGS "--cpu Cortex-M33 --fpu FPv5-SP --endian=little")

# ---------------------------------------------------------------------------
# Compiler flags (INIT values are written to cache on first configure only)
# ---------------------------------------------------------------------------
set(CMAKE_C_FLAGS_INIT   "${_CPU_FLAGS} --dlib_config full -e" CACHE INTERNAL "")
set(CMAKE_ASM_FLAGS_INIT "${_CPU_FLAGS} -s+" CACHE INTERNAL "")

# Debug — no optimisation, full debug information
set(CMAKE_C_FLAGS_DEBUG   "--debug -On"
    CACHE STRING "IAR C flags for Debug build")
set(CMAKE_ASM_FLAGS_DEBUG "--debug"
    CACHE STRING "IAR ASM flags for Debug build")

# Release — optimise for size, no debug information
set(CMAKE_C_FLAGS_RELEASE "-Ohz --remarks"
    CACHE STRING "IAR C flags for Release build")
set(CMAKE_ASM_FLAGS_RELEASE ""
    CACHE STRING "IAR ASM flags for Release build")

# ---------------------------------------------------------------------------
# Executable output extension
# ---------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".out" CACHE INTERNAL "")

# ---------------------------------------------------------------------------
# Sysroot — keep CMake from searching host system paths for libraries
# ---------------------------------------------------------------------------
set(CMAKE_FIND_ROOT_PATH             "${IAR_TOOLCHAIN_PATH}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
