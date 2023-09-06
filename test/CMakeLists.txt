add_executable(test main.cpp math/vector3_test.cpp)

target_include_directories(test PRIVATE extern)

target_link_libraries(test PRIVATE math_lib)