#include "engine/engine.h"

#include "engine/window.h"

gfx::Engine::Engine(const Window& window)
    : surface_{window.CreateSurface(*instance_)},
      device_{*instance_, *surface_},
      swapchain_{device_, window, *surface_},
      vertex_shader_module_{*device_, vk::ShaderStageFlagBits::eVertex, "shaders/vertex.glsl"},
      fragment_shader_module_{*device_, vk::ShaderStageFlagBits::eFragment, "shaders/fragment.glsl"} {}
