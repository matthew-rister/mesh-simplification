add_library(engine STATIC engine.cpp instance.cpp scene.cpp window.cpp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glfw)

find_package(Vulkan REQUIRED)
target_link_libraries(engine PUBLIC Vulkan::Vulkan)

find_package(VulkanHeaders CONFIG REQUIRED)
target_link_libraries(engine PUBLIC Vulkan::Headers)

target_compile_definitions(engine PUBLIC GLFW_INCLUDE_VULKAN
                                  PUBLIC VULKAN_HPP_NO_CONSTRUCTORS
                                  PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(engine PUBLIC include)
