#ifndef SRC_GAME_GAME_H_
#define SRC_GAME_GAME_H_

#include "graphics/camera.h"
#include "graphics/engine.h"
#include "graphics/mesh.h"
#include "graphics/window.h"

class Game {
public:
  Game();

  void Run();

private:
  void HandleKeyEvent(const int key, const int action) const;
  void HandleCursorEvent(const float x, const float y);
  void HandleScrollEvent(const float y);

  gfx::Window window_;
  gfx::Engine engine_;
  gfx::Camera camera_;
  gfx::Mesh mesh_;
};

#endif  // SRC_GAME_GAME_H_
