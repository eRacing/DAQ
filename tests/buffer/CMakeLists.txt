project(test_buffer)

add_executable(entry_init_test
    entry_init.c
    ${DAQ_SOURCES}/buffer.c
)
add_test(entry_init entry_init_test)

add_executable(buffer_init_test
    buffer_init.c
    ${DAQ_SOURCES}/buffer.c
)
add_test(buffer_init buffer_init_test)

add_executable(buffer_insert_test
    buffer_insert.c
    ${DAQ_SOURCES}/buffer.c
)
add_test(buffer_insert buffer_insert_test)

add_executable(buffer_flush_test
    buffer_flush.c
    ${DAQ_SOURCES}/buffer.c
)
add_test(buffer_flush buffer_flush_test)

add_executable(circular_init_test
    circular_init.c
    ${DAQ_SOURCES}/buffer.c
)
add_test(circular_init circular_init_test)

add_executable(circular_insert_test
    circular_insert.c
    ${DAQ_SOURCES}/buffer.c
)
add_test(circular_insert circular_insert_test)
