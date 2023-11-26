add_executable(mesh_simplification main.cpp
                                   game/game.cpp
                                   graphics/arcball.cpp
                                   graphics/device.cpp
                                   graphics/engine.cpp
                                   graphics/glslang_compiler.cpp
                                   graphics/image.cpp
                                   graphics/instance.cpp
                                   graphics/memory.cpp
                                   graphics/obj_loader.cpp
                                   graphics/shader_module.cpp
                                   graphics/swapchain.cpp
                                   graphics/window.cpp)

find_package(VulkanHeaders CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(mesh_simplification PRIVATE Vulkan::Headers
                                                  glfw
                                                  glm::glm
                                                  glslang::glslang
                                                  glslang::glslang-default-resource-limits
                                                  glslang::SPIRV)

target_compile_definitions(mesh_simplification PRIVATE GLFW_INCLUDE_VULKAN
                                                       GLM_ENABLE_EXPERIMENTAL
                                                       GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
                                                       GLM_FORCE_DEPTH_ZERO_TO_ONE
                                                       GLM_FORCE_XYZW_ONLY
                                                       VULKAN_HPP_NO_CONSTRUCTORS
                                                       VULKAN_HPP_NO_SPACESHIP_OPERATOR
                                                       VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_include_directories(mesh_simplification PRIVATE game/include
                                                       graphics/include)

add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
                                                                         ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(mesh_simplification copy_assets)
