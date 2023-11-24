#include <cstdlib>
#include <exception>
#include <iostream>

#include "graphics/engine.h"
#include "graphics/scene.h"
#include "graphics/window.h"

void Run() {
  gfx::Window window{"Mesh Simplification", 1920, 1080};
  gfx::Engine engine{window};
  gfx::Scene scene{engine, &window};

  while (!window.IsClosed()) {
    gfx::Window::Update();
    engine.Render(scene);
  }

  engine.device()->waitIdle();
}

int main() {
  try {
    Run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
