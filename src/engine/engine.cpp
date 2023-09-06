#include "engine.h"
#include "scene.h"

void gfx::Engine::Render(const Scene& scene) const {
    scene.Render();
}