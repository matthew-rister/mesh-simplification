#include "engine.h"

#include "window.h"

gfx::Engine::Engine(const Window& window) : surface_{window.CreateVulkanSurface(*instance_)} {}
