#ifndef SRC_GAME_INCLUDE_GAME_GAME_H_
#define SRC_GAME_INCLUDE_GAME_GAME_H_

#include "engine/engine.h"
#include "engine/window.h"

namespace gfx {

class Game {
public:
  Game() noexcept;

  void Run();

private:
  Window window_;
  Engine engine_;
};

}  // namespace gfx

#endif  // SRC_GAME_INCLUDE_GAME_GAME_H_
