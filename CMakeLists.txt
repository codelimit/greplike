cmake_minimum_required(VERSION 3.12)

project(spot)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(tool)
