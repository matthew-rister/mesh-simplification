#include "engine.h"

#include "window.h"

gfx::Engine::Engine(const Window& window)
    : surface_{window.CreateSurface(*instance_)}, device_{*instance_, *surface_} {}
