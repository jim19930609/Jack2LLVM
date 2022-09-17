cmake_minimum_required(VERSION 3.17)

project(jack2llvm)

set(CMAKE_CXX_STANDARD 14)

find_package(Java)

# Prepare antlr lexer and parser
set(LANG_DIRECTORY "${CMAKE_SOURCE_DIR}/src/language_src")
execute_process(
  COMMAND 
  mkdir -p ${LANG_DIRECTORY}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

execute_process(
  COMMAND 
  ${Java_JAVA_EXECUTABLE} -jar ${CMAKE_SOURCE_DIR}/third_party/antlr-4.9.3-complete.jar -Dlanguage=Cpp -o ${LANG_DIRECTORY} ${CMAKE_SOURCE_DIR}/src/Jack.g4 -no-listener -visitor
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

# Compile source files
add_library(language_lib OBJECT
    gui/gui.cpp
    gui/android.cpp
    gui/cocoa.cpp
    gui/win32.cpp
    gui/x11.cpp
  )

if(TI_WITH_GGUI AND NOT ANDROID)
  target_sources(taichi_ui PRIVATE common/window_base.cpp)
endif()

target_include_directories(taichi_ui
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/external/glfw/include
    ${PROJECT_SOURCE_DIR}/external/eigen
    ${PROJECT_SOURCE_DIR}/external/volk
    ${PROJECT_SOURCE_DIR}/external/Vulkan-Headers/include
    ${LLVM_INCLUDE_DIRS}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/external/glm
  )

target_link_libraries(taichi_ui PRIVATE taichi_core)
target_link_libraries(taichi_ui PRIVATE taichi_common)
