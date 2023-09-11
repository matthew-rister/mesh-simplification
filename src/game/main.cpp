#include <exception>
#include <iostream>

#include "game.h"
#include "vector3.h"

int main() {
    try {
        std::cout << gfx::Vector3{1.0, 2.0f, 3.0f} << std::endl;
        constexpr gfx::Game kGame{};
        kGame.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "An unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
