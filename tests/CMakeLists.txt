add_executable(mesh_simplification_tests geometry/face_test.cpp
                                         geometry/half_edge_mesh_test.cpp
                                         geometry/half_edge_test.cpp
                                         geometry/vertex_test.cpp
                                         graphics/math/spherical_coordinates_test.cpp
                                         graphics/obj_loader_test.cpp)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(mesh_simplification_tests PRIVATE GTest::gtest_main
                                                        geometry
                                                        graphics)

target_include_directories(mesh_simplification_tests PRIVATE ${CMAKE_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(mesh_simplification_tests)
