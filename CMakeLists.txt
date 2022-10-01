cmake_minimum_required(VERSION 3.17)

project(jack2llvm)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET default)

################################
# Prepare third party packages #
################################
if(APPLE)
    set(LLVM_DIR ${CMAKE_SOURCE_DIR}/third_party/llvm-10.0.0-m1)
    set(ANTLR_RUNTIME_DIR ${CMAKE_SOURCE_DIR}/third_party/antlr_runtime_lib/runtime-osx/)
else()
    set(LLVM_DIR ${CMAKE_SOURCE_DIR}/third_party/taichi-llvm-10.0.0-linux)
    set(ANTLR_RUNTIME_DIR ${CMAKE_SOURCE_DIR}/third_party/antlr_runtime_lib/runtime-linux/)
endif()
set(LLVM_CONFIG_BINARY ${LLVM_DIR}/bin/llvm-config)

find_package(Java REQUIRED)
find_package(glog REQUIRED)

# Prepare antlr lexer and parser source codes
set(LANG_DIRECTORY ${CMAKE_SOURCE_DIR}/src/language_src)
file(MAKE_DIRECTORY ${LANG_DIRECTORY})

execute_process(
  COMMAND 
  ${Java_JAVA_EXECUTABLE} -jar ${CMAKE_SOURCE_DIR}/third_party/antlr-4.9.3-complete.jar -Dlanguage=Cpp -o ${LANG_DIRECTORY} ${CMAKE_SOURCE_DIR}/src/Jack.g4 -no-listener -visitor
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

###############################################
# Compile antlr lexer and parser source codes #
###############################################
add_library(language_lib OBJECT
    ${LANG_DIRECTORY}/JackLexer.cpp
    ${LANG_DIRECTORY}/JackParser.cpp
    ${LANG_DIRECTORY}/JackVisitor.cpp
  )

target_include_directories(language_lib
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${LANG_DIRECTORY}
    ${ANTLR_RUNTIME_DIR}/antlr4-runtime
  )

#########################
# Compile Jack compiler #
#########################
execute_process(COMMAND ${LLVM_CONFIG_BINARY} --cflags OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(LLVM_INCLUDE_DIR UNIX_COMMAND ${LLVM_INCLUDE_DIR})

add_library(jack_lib OBJECT
    ${CMAKE_SOURCE_DIR}/src/JackExpressionVisitor.cpp
    ${CMAKE_SOURCE_DIR}/src/JackStatementVisitor.cpp
    ${CMAKE_SOURCE_DIR}/src/JackClassVisitor.cpp
  )
target_compile_options(jack_lib PUBLIC ${LLVM_INCLUDE_DIR})
target_link_libraries (jack_lib glog::glog)

target_include_directories(jack_lib
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${LANG_DIRECTORY}
    ${ANTLR_RUNTIME_DIR}/antlr4-runtime
  )

###############################
# Compile compiler executable #
###############################
execute_process(COMMAND ${LLVM_CONFIG_BINARY} --ldflags --libs core OUTPUT_VARIABLE LLVM_LINK_OPTIONS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(LLVM_LINK_OPTIONS UNIX_COMMAND ${LLVM_LINK_OPTIONS})

add_executable(jack2llvm ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_link_libraries(jack2llvm jack_lib language_lib)
target_link_libraries(jack2llvm ${ANTLR_RUNTIME_DIR}/lib/libantlr4-runtime.a)
target_link_libraries(jack2llvm glog::glog)

target_include_directories(jack2llvm
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${LANG_DIRECTORY}
    ${ANTLR_RUNTIME_DIR}/antlr4-runtime
  )

target_compile_options(jack2llvm PUBLIC ${LLVM_INCLUDE_DIR})
target_link_options(jack2llvm PUBLIC ${LLVM_LINK_OPTIONS})

# Build test cases
include(cmake/Test.cmake)
add_subdirectory(tests)
