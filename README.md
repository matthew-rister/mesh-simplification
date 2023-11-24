# Mesh Simplification

A work in progress implementation of Surface Simplification using Quadric Error Metrics in Vulkan and C++.

## Prerequisites

This project requires CMake 3.27 and a compiler that supports the C++23 language standard. To facilitate CMake configuration, building, and testing, [CMake Presets](https://cmake.org/cmake/help/v3.22/manual/cmake-presets.7.html) are used with [ninja](https://ninja-build.org/) as a build generator.

### Vulkan

This project requires a graphics driver with Vulkan 1.3 support. Because it uses [Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp) which provides a dynamic loader implementation, it is unnecessary to manually link against `vulkan-1.lib`. As a result, it is not required to download the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) for release builds, however, it is still required for debug builds which enable [validation layers](https://vulkan.lunarg.com/doc/view/latest/windows/validation_layers.html) by default.

### Package Management

This project uses [`vcpkg`](https://vcpkg.io) to manage external dependencies.  To get started, run `git submodule update --init` to clone `vcpkg` as a git submodule. Upon completion, CMake will integrate with `vcpkg` to download, compile, and link external libraries specified in the [vcpkg.json](vcpkg.json) manifest when building the project.

### Address Sanitizer

This project enables [Address Sanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) (ASan) for debug builds. On Linux, this should already be available when using a version of GCC or Clang with C++23 support. On Windows, ASan needs to be installed separately which is documented [here](https://learn.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-170#install-addresssanitizer).

## Build

The simplest way to build the project is to use an IDE with CMake integration. Alternatively, the project can be built from the command line using CMake presets. To use the `x64-release` preset, run:

```bash
cmake --preset x64-release
cmake --build --preset x64-release
```

A list of available configuration and build presets can be displayed by running  `cmake --list-presets` and `cmake --build --list-presets` respectively. Note that on Windows, `cl` and `ninja` are expected to be available in your environment path which are available by default when using the Developer Command Prompt for Visual Studio.

## Test

This project uses [Google Test](https://github.com/google/googletest) for unit testing which can be run after building the project with [CTest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html). To use the `x64-release` preset, run:

```bash
ctest --preset x64-release
```

To see what test presets are available, run `ctest --list-presets`.  Alternatively, tests can be run from the separate `tests` executable which is built with the project.
