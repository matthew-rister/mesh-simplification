add_library(engine STATIC device.cpp
                          engine.cpp
                          glslang_compiler.cpp
                          image.cpp
                          instance.cpp
                          memory.cpp
                          shader_module.cpp
                          swapchain.cpp
                          window.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)

target_link_libraries(engine PRIVATE dbgasan warnings)
target_link_libraries(engine PUBLIC glfw
                                    glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV
                                    Vulkan::Headers)

target_compile_definitions(engine PUBLIC GLFW_INCLUDE_VULKAN
                                  PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
                                  PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE
                                  PUBLIC VULKAN_HPP_NO_CONSTRUCTORS
                                  PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(engine PUBLIC include)
