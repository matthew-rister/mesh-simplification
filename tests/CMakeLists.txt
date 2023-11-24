add_executable(hello_test hello_test.cpp)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(hello_test PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(hello_test)
