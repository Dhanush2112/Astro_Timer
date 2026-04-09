# CMake for module test and app runner
add_executable(app_main ${CMAKE_SOURCE_DIR}/Code/App/System/App_Main.c)
add_executable(module_test ${CMAKE_SOURCE_DIR}/ModuleTest/ModulesTest_Main.c)
add_executable(unit_tests ${CMAKE_SOURCE_DIR}/ModuleTest/unit_tests.c)

target_link_libraries(app_main PRIVATE astrotimer)
target_link_libraries(module_test PRIVATE astrotimer)
target_link_libraries(unit_tests PRIVATE astrotimer)

target_include_directories(app_main PRIVATE ${CMAKE_SOURCE_DIR}/Code/App)
target_include_directories(module_test PRIVATE ${CMAKE_SOURCE_DIR}/Code/App)
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/Code/App)

# Enable CTest and register unit tests
include(CTest)
enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)