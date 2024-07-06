cmake_minimum_required(VERSION 3.12)

set(spot_tests_src
        all_found.cpp
        none_found.cpp
        starts_with.cpp
        ends_with.cpp
        star.cpp
        dot.cpp
        complex.cpp
)

create_test_sourcelist(test_sources CommonCxxTests.cxx ${spot_tests_src})

add_executable(spot_tests ${test_sources})
target_link_libraries(spot_tests PRIVATE spot_lib)

set(fail_regex "failed")
foreach (test ${spot_tests_src})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${test_name} COMMAND spot_tests ${test_name} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/sandbox)
    set_tests_properties(${test_name} PROPERTIES
            FAIL_REGULAR_EXPRESSION "${fail_regex}"
            #TIMEOUT 1
    )
endforeach ()