add_executable(game main.cpp game.cpp)

target_include_directories(game PRIVATE include)

target_link_libraries(game PRIVATE engine math)