add_library(Engine STATIC engine.cpp instance.cpp scene.cpp window.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)

target_link_libraries(Engine PRIVATE EnableAllWarningsAsErrors EnableAsanIfDebug)
target_link_libraries(Engine PUBLIC glfw Vulkan::Vulkan Vulkan::Headers)

target_compile_definitions(Engine PUBLIC GLFW_INCLUDE_VULKAN
                                  PUBLIC VULKAN_HPP_NO_CONSTRUCTORS
                                  PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(Engine PUBLIC include)
