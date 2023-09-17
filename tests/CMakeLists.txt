find_package(Catch2 CONFIG REQUIRED)

add_executable(tests math/vector3_test.cpp)

target_link_libraries(tests PRIVATE math Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)
