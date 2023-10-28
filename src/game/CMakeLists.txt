add_executable(game main.cpp game.cpp)

target_link_libraries(game PRIVATE dbgasan engine warnings)

target_include_directories(game PRIVATE include)

get_target_property(ENGINE_SOURCE_DIR engine SOURCE_DIR)
get_target_property(GAME_BINARY_DIR game BINARY_DIR)

add_custom_target(copy_shaders COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_SOURCE_DIR}/shaders
                                                                          ${GAME_BINARY_DIR}/shaders)
add_dependencies(game copy_shaders)
