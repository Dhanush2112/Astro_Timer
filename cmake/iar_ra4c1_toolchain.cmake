# Minimal IAR toolchain file for Renesas RA4C1
# Requirements: `iccarm` (IAR C compiler) and `ilinkarm` (IAR linker) should be on PATH when running CMake,
# or set the full paths via the variables below before configuring.
# This file should be passed to CMake via -DCMAKE_TOOLCHAIN_FILE=cmake/iar_ra4c1_toolchain.cmake

# Treat the target as a generic embedded system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Prevent CMake from trying to link/run test executables during the configure checks
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# User-overridable variables (set on cmake command line or in toolchain file)
# Example usage on the command line:
# cmake -S . -B build-iar -DCMAKE_TOOLCHAIN_FILE=cmake/iar_ra4c1_toolchain.cmake -DEMBEDDED=ON -DICCARM_EXEC="C:/Path/to/iccarm.exe" -DILINKARM_EXEC="C:/Path/to/ilinkarm.exe" -DIAR_ICF="${CMAKE_SOURCE_DIR}/Code/Config/iar_ra4c1.icf"

if(NOT DEFINED ICCARM_EXEC)
    find_program(ICCARM_EXEC iccarm HINTS ENV PATH)
endif()
if(NOT DEFINED ILINKARM_EXEC)
    find_program(ILINKARM_EXEC ilinkarm HINTS ENV PATH)
endif()

# Default CPU for RA4C1 (Cortex-M23). Adjust as needed for your variant.
if(NOT DEFINED IAR_CPU)
    set(IAR_CPU "Cortex-M23")
endif()

# Default flags for iccarm and ilinkarm. Tweak these per your project.
if(NOT DEFINED ICCARM_FLAGS)
    # Common: set CPU and generate debug info with --dlib_config or other flags as needed
    set(ICCARM_FLAGS "--cpu=${IAR_CPU} --no_cpp");
endif()
if(NOT DEFINED ILINKARM_FLAGS)
    set(ILINKARM_FLAGS "");
endif()

# Optional: path to ICF (IAR linker configuration) file. Leave empty to pass none.
if(NOT DEFINED IAR_ICF)
    set(IAR_ICF "")
endif()

# Configure the compiler variables used by CMake
if(ICCARM_EXEC)
    set(CMAKE_C_COMPILER "${ICCARM_EXEC}")
    set(CMAKE_ASM_COMPILER "${ICCARM_EXEC}")
    set(CMAKE_C_COMPILER_WORKS TRUE)
else()
    message(WARNING "iccarm not found on PATH and ICCARM_EXEC not provided. CMake configure may fail.")
endif()

# Provide convenient variables for use by build wrapper targets
set(ICCARM_EXEC "${ICCARM_EXEC}" CACHE PATH "Path to iccarm executable")
set(ILINKARM_EXEC "${ILINKARM_EXEC}" CACHE PATH "Path to ilinkarm executable")
set(ICCARM_FLAGS "${ICCARM_FLAGS}" CACHE STRING "Flags to pass to iccarm")
set(ILINKARM_FLAGS "${ILINKARM_FLAGS}" CACHE STRING "Flags to pass to ilinkarm")
set(IAR_ICF "${IAR_ICF}" CACHE FILEPATH "Path to IAR .icf linker configuration file (optional)")

# Provide default C flags so targets pick up CPU setting if not overridden
if(NOT DEFINED CMAKE_C_FLAGS)
    set(CMAKE_C_FLAGS "${ICCARM_FLAGS}")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ICCARM_FLAGS}")
endif()

# Mark as cross compiling
set(CMAKE_CROSSCOMPILING TRUE)

# End of toolchain file