#pragma once

#include "engine.h"
#include "window.h"

namespace gfx {

class Game {
public:
  Game() noexcept;

  void Run() const;

private:
  Window window_;
  Engine engine_;
};

}  // namespace gfx
