#include <cstdlib>
#include <exception>
#include <iostream>

#include "game/game.h"

int main() {
  try {
    Game game;
    game.Run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
