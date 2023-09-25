add_library(engine STATIC engine.cpp scene.cpp window.cpp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glfw)

target_include_directories(engine PUBLIC include)
