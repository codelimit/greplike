cmake_minimum_required(VERSION 3.12)

add_executable(spot_tool main.cpp)
target_link_libraries(spot_tool PRIVATE spot_lib)
set_target_properties(spot_tool PROPERTIES OUTPUT_NAME "spot")