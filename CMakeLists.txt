cmake_minimum_required(VERSION 3.15)
project(Astro_Timer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(EMBEDDED "Build for embedded target (expose app_main as entry, don't define host main wrappers)" OFF)
if(EMBEDDED)
    add_compile_definitions(EMBEDDED=1)
endif()

# Build code and tests
add_subdirectory(Code)

# Only add module tests when not building for embedded target. Embedded builds
# should use a cross toolchain (IAR) and shouldn't attempt to compile/run
# host test executables.
if(NOT EMBEDDED)
    add_subdirectory(ModuleTest)
else()
    # Include optional IAR helper targets for embedded builds
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/iar_build_targets.cmake")
        include(cmake/iar_build_targets.cmake)
    endif()
endif()