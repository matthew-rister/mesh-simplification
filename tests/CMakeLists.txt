add_executable(tests engine/data_view_test.cpp)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(tests PRIVATE dbgasan engine warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
