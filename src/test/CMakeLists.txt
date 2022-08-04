add_executable(test_tread_pool trivial_test_tread_pool.cpp)
#add_executable(test_client_list trivial_test_client_list.cpp)
add_executable(test_server trivial_test_server.cpp)
add_executable(test_client_handler trivial_test_client_handler.cpp )


add_executable(test_sequence test_sequence.cpp)
add_test(NAME SEQUENSE_TEST COMMAND test_sequence 50 80)
set_property(TEST SEQUENSE_TEST PROPERTY PASS_REGULAR_EXPRESSION "50 130 210 50 130 210 50 130 210 50"  )