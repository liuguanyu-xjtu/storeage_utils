if(NOT TARGET gtest)
    add_subdirectory(gtest)
endif()

if(NOT TARGET absl_str_format)
    add_subdirectory(absl)
endif()