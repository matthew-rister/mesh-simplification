#pragma once

#include "engine.h"
#include "scene.h"

namespace gfx {

class Game {

public:
    void Run() const;

private:
    Engine engine_;
    Scene scene_;
};
}
