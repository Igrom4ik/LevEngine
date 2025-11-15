# Minimal placeholder CMake for GLFW
cmake_minimum_required(VERSION 3.10)
project(glfw_placeholder)

add_library(glfw INTERFACE)
target_include_directories(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Provide an imported target name 'glfw' to match expectations
set_target_properties(glfw PROPERTIES EXPORT_NAME glfw)

