cmake_minimum_required(VERSION 3.20.4)
project(storage_utils LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(third)

function(su_test test_file lib)
    set(extra_args ${ARGN})
    get_filename_component(test_target_name ${test_file} NAME_WE)
    set_source_files_properties(${test_file} PROPERTIES COMPILE_FLAGS "-fno-access-control")

    add_executable(${test_target_name} ${test_file})
    target_link_libraries(${test_target_name}
            ${lib}
            gtest_main
            gmock_main)
    add_test(NAME ${test_target_name} COMMAND ${test_target_name})
endfunction()
enable_testing()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/gtest/googletest/include)
add_subdirectory(compress_range_map)