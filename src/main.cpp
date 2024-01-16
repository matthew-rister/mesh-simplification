#include <cstdlib>
#include <exception>
#include <iostream>

#include "app.h"  // NOLINT(build/include_subdir)

int main() {
  try {
    gfx::App app;
    app.Run();
  } catch (const std::system_error& e) {
    std::cerr << '[' << e.code() << "] " << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
