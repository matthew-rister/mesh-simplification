add_executable(hello_test hello_test.cpp)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(hello_test PRIVATE enable_all_warnings_as_errors enable_asan_if_debug GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(hello_test)