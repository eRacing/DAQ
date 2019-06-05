project(test_buffer)

add_executable(ringbuffer_init_test
    ringbuffer_init.c
    message.c
)
add_test(ringbuffer_init ringbuffer_init_test)

add_executable(ringbuffer_insert_test
    ringbuffer_insert.c
    message.c
)
add_test(ringbuffer_insert ringbuffer_insert_test)
#
#add_executable(buffer_flush_test
#    buffer_flush.c
#    ${DAQ_SOURCES}/buffer.c
#)
#add_test(buffer_flush buffer_flush_test)
#

#
#add_executable(circular_insert_test
#    circular_insert.c
#    ${DAQ_SOURCES}/buffer.c
#)
#add_test(circular_insert circular_insert_test)
