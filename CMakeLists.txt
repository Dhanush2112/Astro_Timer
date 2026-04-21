# CMakeLists.txt
# Root build script for Astro Timer firmware.
#
# Firmware build (IAR, default):
#   cmake --preset debug    then  cmake --build --preset debug
#   cmake --preset release  then  cmake --build --preset release
#
# Module tests (host GCC):
#   cmake --preset test     then  cmake --build --preset test
#   ctest --preset test

cmake_minimum_required(VERSION 3.25)

# ---------------------------------------------------------------------------
# App and HAL source lists - defined here once, inherited by subdirectories.
# ---------------------------------------------------------------------------
set(APP_SOURCES
    ${CMAKE_SOURCE_DIR}/Code/App/Scheduler.c
    ${CMAKE_SOURCE_DIR}/Code/App/Astro/Astro.c
    ${CMAKE_SOURCE_DIR}/Code/App/CfgDataStore/ParameterBlock.c
    ${CMAKE_SOURCE_DIR}/Code/App/CfgDataStore/WriteChecks.c
    ${CMAKE_SOURCE_DIR}/Code/App/DateTime/DateTime.c
    ${CMAKE_SOURCE_DIR}/Code/App/LUI/LUI.c
)

set(APP_INCLUDES
    ${CMAKE_SOURCE_DIR}/Code
    ${CMAKE_SOURCE_DIR}/Code/App
)

set(HAL_SOURCES
    ${CMAKE_SOURCE_DIR}/Code/HAL/RA4C1/main.c
    ${CMAKE_SOURCE_DIR}/Code/HAL/RA4C1/HAL_Display/HAL_Display.c
    ${CMAKE_SOURCE_DIR}/Code/HAL/RA4C1/HAL_GPIO/HAL_GPIO.c
    ${CMAKE_SOURCE_DIR}/Code/HAL/RA4C1/HAL_NVM/HAL_NVM.c
    ${CMAKE_SOURCE_DIR}/Code/HAL/RA4C1/HAL_RTC/HAL_RTC.c
)

# ---------------------------------------------------------------------------
# Build mode: set BUILD_TESTS=ON (via preset) to compile and run host tests.
# Default OFF -> firmware build with IAR toolchain.
# ---------------------------------------------------------------------------
option(BUILD_TESTS "Build module tests on the host using GCC" OFF)

if(BUILD_TESTS)
    # -----------------------------------------------------------------------
    # Module test build  -  host GCC, no IAR toolchain required
    # -----------------------------------------------------------------------
    project(AstroTimerModuleTest LANGUAGES C)
    enable_testing()
    add_subdirectory(Code/ModuleTest)
else()
    # -----------------------------------------------------------------------
    # Firmware build  -  IAR ARM toolchain (passed via CMAKE_TOOLCHAIN_FILE)
    # -----------------------------------------------------------------------
    project(AstroTimer LANGUAGES C ASM)

    # Firmware version - bump these numbers for each release.
    # Output file:  Astro_<FW_MAJOR>.<FW_MINOR>.hex / .bin
    set(FW_MAJOR "01")
    set(FW_MINOR "00")

    set(TARGET_EXE  "Astro")
    set(OUTPUT_NAME "Astro_${FW_MAJOR}.${FW_MINOR}")

    add_executable(${TARGET_EXE}
        ${APP_SOURCES}
        ${HAL_SOURCES}
    )

    set_target_properties(${TARGET_EXE} PROPERTIES
        OUTPUT_NAME "${OUTPUT_NAME}"
    )

    target_include_directories(${TARGET_EXE} PRIVATE
        ${APP_INCLUDES}
        ${CMAKE_SOURCE_DIR}/Code/HAL/RA4C1
    )

    target_compile_definitions(${TARGET_EXE} PRIVATE
        FW_MAJOR=${FW_MAJOR}
        FW_MINOR=${FW_MINOR}
    )

    target_link_options(${TARGET_EXE} PRIVATE
        --config "${CMAKE_SOURCE_DIR}/Code/HAL/RA4C1/RA4C1.icf"
        --map    "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}.map"
    )

    set(IELFTOOL "${IAR_TOOLCHAIN_PATH}/bin/ielftool.exe")

    add_custom_command(TARGET ${TARGET_EXE} POST_BUILD
        COMMAND "${IELFTOOL}" --ihex
                "$<TARGET_FILE:${TARGET_EXE}>"
                "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}.hex"
        COMMAND "${IELFTOOL}" --bin
                "$<TARGET_FILE:${TARGET_EXE}>"
                "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}.bin"
        COMMENT "Generating ${OUTPUT_NAME}.hex and ${OUTPUT_NAME}.bin"
        VERBATIM
    )
endif()
