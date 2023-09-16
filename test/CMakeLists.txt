find_package(Catch2 CONFIG REQUIRED)

add_executable(test math/vector3_test.cpp)

target_link_libraries(test PRIVATE math Catch2::Catch2WithMain)