#pragma once

#include "instance.h"

namespace gfx {
class Scene;

class Engine {
public:
  void Render(const Scene&) const;

private:
  Instance instance_;
};

}  // namespace gfx
