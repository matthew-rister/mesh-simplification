#ifndef SRC_GAME_GAME_H_
#define SRC_GAME_GAME_H_

#include "graphics/camera.h"
#include "graphics/engine.h"
#include "graphics/mesh.h"
#include "graphics/window.h"

namespace gfx {

class Game {
public:
  Game();

  void Run();

private:
  void HandleKeyEvent(const int key, const int action);
  void HandleCursorEvent(const float x, const float y);
  void HandleScrollEvent(const float y);

  Window window_;
  Engine engine_;
  Camera camera_;
  Mesh mesh_;
};

}  // namespace gfx

#endif  // SRC_GAME_GAME_H_
