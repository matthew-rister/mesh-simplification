add_executable(game main.cpp game.cpp)

target_link_libraries(game PRIVATE dbgasan graphics warnings)

target_include_directories(game PRIVATE include)

get_target_property(GRAPHICS_SOURCE_DIR graphics SOURCE_DIR)
get_target_property(GAME_BINARY_DIR game BINARY_DIR)

add_custom_target(copy_shaders COMMAND ${CMAKE_COMMAND} -E copy_directory ${GRAPHICS_SOURCE_DIR}/shaders
                                                                          ${GAME_BINARY_DIR}/shaders)
add_dependencies(game copy_shaders)
