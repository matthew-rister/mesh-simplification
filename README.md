
# VkRender

A cross-platform Vulkan renderer written in C++20.

## Building

This project requires CMake 3.22 and a C++ compiler that supports the C++20 language standard. It has been tested with
the Microsoft C/C++ Compiler Version 19.37.32822 on Windows and Clang 17 on Ubuntu 22.04.3 LTS. To facilitate CMake
configuration, this project utilizes [CMake Presets](https://cmake.org/cmake/help/v3.22/manual/cmake-presets.7.html). To
see what presets are available run `cmake --list-presets`. Replacing `x64-release` with your desired preset, the project
 can then be built with the following CMake command:

 ```cmake -S . --preset x64-release && cmake --build out/build/x64-release```

## Testing

This project uses [Catch2](https://github.com/catchorg/Catch2) for unit testing. Tests can be run from a separate
executable which is built with the project.

## Style

### C++ Style Guide

This project follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with a few
exceptions (e.g., 120 character limit, 4 space indent-width). Style is enforced across different platforms using
 [clang-format](https://clang.llvm.org/docs/ClangFormatStyleOptions.html).

### Git

To encourage descriptive commit messages, this project uses a commit message template which can be configured by running
`git config commit.template .gitmessage`. This template includes 4 parts: a short-one line summary using the
[semantic commit message](https://gist.github.com/joshbuchea/6f47e86d2510bce28f8e7f42ae84c716) format, a description
of the problem being solved, a description of the solution to the problem, and relevant notes such as testing or issues
closed by this commit.