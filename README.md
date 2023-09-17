
# VkRender

A cross-platform Vulkan renderer written in C++20.

## Engine Features
TODO

## How to Run

### Prerequisites 
This project requires [CMake](https://cmake.org/) 3.22 and a C++ compiler that supports the C++20 language standard. It has been tested with the Microsoft C/C++ Compiler Version 19.37.32822 on Windows and Clang 17 on Ubuntu 22.04.3 LTS.  

### Package Management
This project uses [vcpkg](https://vcpkg.io) as a git submodule to manage external dependencies. To initialize vcpkg, run `git submodule update --init` followed by `./vcpkg/bootstrap-vcpkg.sh` (Linux) or `.\vcpkg\bootstrap-vcpk.bat` (Windows).

### Address Sanitizer
This project enables [Address Sanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) (ASan) for debug builds. Because the project requires a C++20 compiler, ASan should be available when using either GCC or Clang. On Windows, ASan needs to be installed separately which is documented [here](https://learn.microsoft.com/en-us/cpp/sanitizers/asan).

### Building 
This project utilizes [CMake Presets](https://cmake.org/cmake/help/v3.22/manual/cmake-presets.7.html) to facilitate CMake configuration.  To build the project from the command line using the `x64-release` preset, run `cmake -S . --preset x64-release && cmake --build out/build/x64-release`. To see what presets are available, run `cmake --list-presets`.  Alternatively, you can use an IDE with CMake integration.

### Testing
This project uses [Catch2](https://github.com/catchorg/Catch2) for unit testing. After building the project, these can be run from the separate `tests` executable or by running `ctest` from the project build directory.

## Contributing

### C++ Style Guide 
This project follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with a few exceptions (e.g., 120 character limit, 4 space indent-width). Style is enforced across different platforms using [clang-format](https://clang.llvm.org/docs/ClangFormatStyleOptions.html).

### Git Commits
To encourage descriptive commit messages, this project uses a commit message template which can be configured by running `git config commit.template .gitmessage`. This template includes 4 parts: a short-one line summary using the [semantic commit message](https://gist.github.com/joshbuchea/6f47e86d2510bce28f8e7f42ae84c716) format, a description of the problem being solved, a description of the solution to the problem, and relevant notes such as testing or issues closed by this commit.
