# Code/ModuleTest/CMakeLists.txt
# Included via add_subdirectory() from the root CMakeLists.txt when BUILD_TESTS=ON.
# Builds one executable per module and registers each with CTest.
# APP_SOURCES and APP_INCLUDES are inherited from the parent scope.
#
# Each per-module test is built from:
#   <TestFile.c>  +  functest_main.c  +  APP_SOURCES
# functest_main.c provides main() which calls Unity''s UnityMain -> RunAllTests().
# No main() lives inside the individual test files.

include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.1
)
set(UNITY_EXTENSION_FIXTURE ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(unity)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wno-unused-parameter
)

# ---------------------------------------------------------------------------
# Helper macro: one test exe = test file + functest_main.c + App sources
# ---------------------------------------------------------------------------
macro(add_module_test NAME TEST_SOURCE)
    add_executable(${NAME}
        ${TEST_SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/functest_main.c
        ${APP_SOURCES}
    )
    target_include_directories(${NAME} PRIVATE ${APP_INCLUDES})
    target_link_libraries(${NAME} PRIVATE unity)
    add_test(NAME ${NAME} COMMAND ${NAME})
endmacro()

# ---------------------------------------------------------------------------
# Per-module tests
# ---------------------------------------------------------------------------
add_module_test(test_astro
    ${CMAKE_CURRENT_SOURCE_DIR}/Test_Astro.c)

add_module_test(test_datetime
    ${CMAKE_CURRENT_SOURCE_DIR}/Test_DateTime.c)

add_module_test(test_scheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/Test_Scheduler.c)

add_module_test(test_cfgdatastore
    ${CMAKE_CURRENT_SOURCE_DIR}/Test_CfgDataStore.c)

