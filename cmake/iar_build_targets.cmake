# Helper CMake functions for building firmware with IAR tools
# Usage: include(cmake/iar_build_targets.cmake) then call
# add_iar_firmware(<target> SOURCES <src1.c;src2.c> OUTPUT <firmware.out> [ICF <path/to/icf>])

function(add_iar_firmware tgt)
    cmake_parse_arguments(AR "" "SOURCES;OUTPUT;ICF" "" ${ARGN})
    if(NOT AR_SOURCES)
        message(FATAL_ERROR "add_iar_firmware: no SOURCES provided")
    endif()
    if(NOT AR_OUTPUT)
        message(FATAL_ERROR "add_iar_firmware: no OUTPUT provided")
    endif()

    # Create object files list in a private object library
    add_library(${tgt}_obj OBJECT ${AR_SOURCES})
    # Ensure the object library uses the project's includes
    target_include_directories(${tgt}_obj PRIVATE ${CMAKE_SOURCE_DIR}/Code/App)

    # Collect object files for linking
    get_target_property(_obj_files ${tgt}_obj OBJECTS)
    if(NOT _obj_files)
        message(FATAL_ERROR "No object files generated for ${tgt}_obj")
    endif()

    # Custom command: use ilinkarm to link the object files into the final image
    set(_link_cmd "${ILINKARM_EXEC} ${ILINKARM_FLAGS}")
    if(AR_ICF)
        set(_link_cmd "${_link_cmd} --config ${AR_ICF}")
    endif()

    # Build the output using a custom target that depends on the object files
    add_custom_command(
        OUTPUT ${AR_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Linking firmware ${AR_OUTPUT} with IAR"
        COMMAND ${_link_cmd} ${_obj_files} -o ${AR_OUTPUT}
        DEPENDS ${_obj_files}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "IAR: linking ${AR_OUTPUT}"
        VERBATIM
    )

    add_custom_target(${tgt} ALL DEPENDS ${AR_OUTPUT})

    # Expose the output as a property of the target
    set_target_properties(${tgt} PROPERTIES OUTPUT_NAME ${AR_OUTPUT})
endfunction()
