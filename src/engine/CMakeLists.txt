add_library(engine STATIC device.cpp engine.cpp instance.cpp window.cpp swapchain.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)

target_link_libraries(engine PRIVATE enable_all_warnings_as_errors enable_asan_if_debug)
target_link_libraries(engine PUBLIC glfw Vulkan::Vulkan Vulkan::Headers)

target_compile_definitions(engine PUBLIC GLFW_INCLUDE_VULKAN
                                  PUBLIC VULKAN_HPP_NO_CONSTRUCTORS
                                  PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(engine PUBLIC include)
