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
    execute_process(COMMAND brew --prefix OUTPUT_VARIABLE BREW_BASE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(LLC_BINARY ${BREW_BASE_DIR}/opt/llvm/bin/llc)
else()
endif()
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jack.o
                       COMMAND 
                       ${CMAKE_SOURCE_DIR}/build/jack2llvm ${JACK_FILES_DIR} > jack.ll
                       COMMAND
                       ${LLC_BINARY} jack.ll -filetype=obj -o ${CMAKE_CURRENT_BINARY_DIR}/jack.o
                       VERBATIM
                       DEPENDS jack2llvm)

    add_executable(${ADD_TEST_NAME} ${ADD_TEST_DRIVER_FILE} ${CMAKE_CURRENT_BINARY_DIR}/jack.o)

endfunction()
