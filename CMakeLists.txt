cmake_minimum_required(VERSION 3.17)

project(jack2llvm)

set(CMAKE_CXX_STANDARD 14)

find_package(Java)

set(LANG_DIRECTORY "${CMAKE_SOURCE_DIR}/build/language_src")

execute_process(
  COMMAND 
  mkdir -p ${LANG_DIRECTORY}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

message("${Java_JAVA_EXECUTABLE} -jar ${CMAKE_SOURCE_DIR}/runtime_lib/antlr-4.9.3-complete.jar -Dlanguage=Cpp -o ${LANG_DIRECTORY} ${CMAKE_SOURCE_DIR}/Jack.g4 -no-listener -visitor")

execute_process(
  COMMAND 
  ${Java_JAVA_EXECUTABLE} -jar ${CMAKE_SOURCE_DIR}/runtime_lib/antlr-4.9.3-complete.jar -Dlanguage=Cpp -o ${LANG_DIRECTORY} ${CMAKE_SOURCE_DIR}/Jack.g4 -no-listener -visitor
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  
