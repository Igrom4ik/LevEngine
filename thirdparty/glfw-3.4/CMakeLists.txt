cmake_minimum_required(VERSION 3.0)
project(glfw_dummy)
add_library(glfw INTERFACE)
# Provide a dummy target with include directory to satisfy top-level includes
target_include_directories(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

