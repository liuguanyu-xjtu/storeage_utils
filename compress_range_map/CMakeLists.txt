
list(APPEND COMPRESS_RANGE_MAP compress_range_map.cc)

add_library(compress_map_obj OBJECT ${COMPRESS_RANGE_MAP}
        bits_stream.h)
add_library(compress_map_lib STATIC $<TARGET_OBJECTS:compress_map_obj>)

su_test("compress_range_map_test.cc" compress_map_lib)

