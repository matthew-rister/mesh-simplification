add_executable(game main.cpp game.cpp)

target_link_libraries(game PRIVATE enable_all_warnings_as_errors enable_asan_if_debug engine)

target_include_directories(game PRIVATE include)
