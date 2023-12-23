add_executable(mesh_simplification main.cpp app.cpp)

target_link_libraries(mesh_simplification PRIVATE asan geometry graphics warnings)

add_subdirectory(geometry)
add_subdirectory(graphics)
add_subdirectory(math)

add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
                                                                         ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(mesh_simplification copy_assets)
