add_executable(mesh_simplification_tests geometry/face_test.cpp
                                         geometry/half_edge_mesh_test.cpp
                                         geometry/half_edge_test.cpp
                                         geometry/vertex_test.cpp
                                         graphics/data_view_test.cpp
                                         graphics/obj_loader_test.cpp
                                         math/spherical_coordinates_test.cpp)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(mesh_simplification_tests PRIVATE GTest::gtest_main
                                                        asan
                                                        geometry
                                                        graphics
                                                        warnings
                                                INTERFACE math)

target_include_directories(mesh_simplification_tests PRIVATE ${CMAKE_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(mesh_simplification_tests)
