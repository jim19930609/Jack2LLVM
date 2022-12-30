include(CMakeParseArguments)

function(ADD_TEST)
    set(options OPTIONAL FAST)
    set(oneValueArgs NAME DRIVER_FILE)
    set(multiValueArgs JACK_FILES)
    cmake_parse_arguments(ADD_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}" )
    
    list(TRANSFORM ADD_TEST_JACK_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
    string(REPLACE ";" " " JACK_FILES_DIR "${ADD_TEST_JACK_FILES}")
    separate_arguments(JACK_FILES_DIR UNIX_COMMAND "${JACK_FILES_DIR}")
    
    if(APPLE)
        set(LLC_BINARY ${CMAKE_SOURCE_DIR}/third_party/llvm-10.0.0-m1/bin/llc)
    else()
        set(LLC_BINARY ${CMAKE_SOURCE_DIR}/third_party/taichi-llvm-10.0.0-linux/bin/llc)
    endif()

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jack.o
                       COMMAND 
                       ${CMAKE_SOURCE_DIR}/build/jack2llvm ${JACK_FILES_DIR} > jack.ll
                       COMMAND
                       ${LLC_BINARY} jack.ll -filetype=obj -o ${CMAKE_CURRENT_BINARY_DIR}/jack.o
                       VERBATIM
                       DEPENDS jack2llvm)
    
    add_executable(${ADD_TEST_NAME} ${ADD_TEST_DRIVER_FILE} ${CMAKE_CURRENT_BINARY_DIR}/jack.o)
    
    set(JACK_TEST_CASES "${CMAKE_CURRENT_BINARY_DIR}/${ADD_TEST_NAME} ${JACK_TEST_CASES}" CACHE INTERNAL "")

endfunction()
