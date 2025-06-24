
list(APPEND THREAD_SRCS async_thread.cc)

add_library(thread_obj OBJECT ${THREAD_SRCS})
add_library(thread_lib STATIC $<TARGET_OBJECTS:thread_obj>)



