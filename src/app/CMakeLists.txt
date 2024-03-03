add_executable(mesh_simplification main.cpp app.cpp)

target_link_libraries(mesh_simplification PRIVATE geometry graphics)
target_include_directories(mesh_simplification PRIVATE include)

add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/assets
                                                                                          ${CMAKE_CURRENT_BINARY_DIR}/assets)
