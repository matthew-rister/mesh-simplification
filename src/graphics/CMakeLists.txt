add_library(graphics STATIC arc_camera.cpp
                            device.cpp
                            engine.cpp
                            glslang_compiler.cpp
                            image.cpp
                            instance.cpp
                            memory.cpp
                            mesh.cpp
                            obj_loader.cpp
                            physical_device.cpp
                            shader_module.cpp
                            swapchain.cpp
                            window.cpp)

find_package(VulkanHeaders CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(graphics PUBLIC Vulkan::Headers
                                      glfw
                                      glm::glm
                                      glslang::glslang
                                      glslang::glslang-default-resource-limits
                                      glslang::SPIRV
                                      math)

target_compile_definitions(graphics PUBLIC GLFW_INCLUDE_VULKAN
                                           GLM_ENABLE_EXPERIMENTAL
                                           GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
                                           GLM_FORCE_DEPTH_ZERO_TO_ONE
                                           GLM_FORCE_XYZW_ONLY
                                           VULKAN_HPP_NO_CONSTRUCTORS
                                           VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(graphics PUBLIC ${SRC_DIR})
