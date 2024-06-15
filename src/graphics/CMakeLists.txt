add_library(graphics STATIC)

target_sources(
  graphics
  # cmake-format: off
  PUBLIC FILE_SET HEADERS
         BASE_DIRS ${SRC_DIR}
         FILES arc_camera.h
               buffer.h
               device.h
               engine.h
               glslang_compiler.h
               image.h
               instance.h
               memory.h
               mesh.h
               obj_loader.h
               physical_device.h
               shader_module.h
               swapchain.h
               window.h
  # cmake-format: on
  PRIVATE arc_camera.cpp
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

target_link_libraries(
  graphics
  PUBLIC Vulkan::Headers
         glfw
         glm::glm
         glslang::glslang
         glslang::glslang-default-resource-limits
         glslang::SPIRV
         math)

target_compile_definitions(
  graphics
  PUBLIC GLFW_INCLUDE_VULKAN
         GLM_ENABLE_EXPERIMENTAL
         GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
         GLM_FORCE_DEPTH_ZERO_TO_ONE
         GLM_FORCE_XYZW_ONLY
         VULKAN_HPP_NO_CONSTRUCTORS
         VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
