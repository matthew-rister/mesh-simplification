add_library(geometry STATIC face.cpp
                            half_edge_mesh.cpp
                            mesh_simplifier.cpp)

find_package(glm CONFIG REQUIRED)

target_link_libraries(geometry PRIVATE asan warnings)
target_link_libraries(geometry PUBLIC glm::glm graphics)

target_compile_definitions(geometry PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
                                           GLM_FORCE_XYZW_ONLY)

target_include_directories(geometry PUBLIC include)
