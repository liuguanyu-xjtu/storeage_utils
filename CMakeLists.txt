cmake_minimum_required(VERSION 3.20.4)
project(storage_utils LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
add_subdirectory(third)


enable_testing()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/gtest/googletest/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/absl/)
add_subdirectory(compress_range_map)
add_subdirectory(sync)
add_subdirectory(thread)

add_library(zeta_ut STATIC
            $<TARGET_OBJECTS:compress_map_obj>
            $<TARGET_OBJECTS:sync_obj>
            $<TARGET_OBJECTS:thread_obj>)

function(su_test test_file)
    set(extra_args ${ARGN})
    get_filename_component(test_target_name ${test_file} NAME_WE)
    set_source_files_properties(${test_file} PROPERTIES COMPILE_FLAGS "-fno-access-control")

    add_executable(${test_target_name} ${test_file})
    target_link_libraries(${test_target_name}
            zeta_ut
            absl_optional
            absl_bad_optional_access
            absl_bad_variant_access
            absl_flat_hash_map
            gtest_main
            gmock_main)
    add_test(NAME ${test_target_name} COMMAND ${test_target_name})
endfunction()

su_test("compress_range_map/compress_range_map_test.cc")
su_test("thread/async_thread_test.cc" thread_lib)


