# CMake for building the core library

file(GLOB_RECURSE APP_C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/App/*.c")

# Exclude the test/app mains in App/System from the library sources
list(FILTER APP_C_SOURCES EXCLUDE REGEX ".*/App/System/.*")

add_library(astrotimer STATIC ${APP_C_SOURCES})

target_include_directories(astrotimer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/App)

# Propagate EMBEDDED compile definition if set
if(DEFINED EMBEDDED)
    target_compile_definitions(astrotimer PRIVATE EMBEDDED=$<BOOL:${EMBEDDED}>)
endif()

set_target_properties(astrotimer PROPERTIES OUTPUT_NAME "astrotimer")