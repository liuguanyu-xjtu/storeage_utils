
list(APPEND SYNC_SRCS event_notifier.cc wait_group.cc)

add_library(sync_obj OBJECT ${SYNC_SRCS})
add_library(sync_lib STATIC $<TARGET_OBJECTS:sync_obj>)

list(APPEND OBJS $<TARGET_OBJECTS:sync_obj>)



