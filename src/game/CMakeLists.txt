add_executable(game main.cpp game.cpp)

target_link_libraries(game PRIVATE dbgasan graphics warnings)

target_include_directories(game PRIVATE include)

add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
                                                                         ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(game copy_assets)
