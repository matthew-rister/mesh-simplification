add_executable(Game main.cpp game.cpp)

target_link_libraries(Game PRIVATE EnableAllWarningsAsErrors EnableAsanIfDebug Engine)

target_include_directories(Game PRIVATE include)
