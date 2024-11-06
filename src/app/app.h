#ifndef APP_APP_H_
#define APP_APP_H_

#include "graphics/arc_camera.h"
#include "graphics/engine.h"
#include "graphics/mesh.h"
#include "graphics/window.h"

namespace gfx {

class App {
public:
  App();

  void Run();

private:
  void OnKeyEvent(int key, int action);
  void OnCursorEvent(float x, float y);
  void OnScrollEvent(float y);

  Window window_;
  Engine engine_;
  ArcCamera camera_;
  Mesh mesh_;
};

}  // namespace gfx

#endif  // APP_APP_H_
