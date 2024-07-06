cmake_minimum_required(VERSION 3.12)

add_library(spot_lib spot.cpp search_engine.cpp)
target_include_directories(spot_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(spot_lib PROPERTIES OUTPUT_NAME "spot")