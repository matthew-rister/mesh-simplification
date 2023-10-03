# VkRender
A cross-platform Vulkan renderer written in C++23.

## Prerequisites
This project requires [CMake](https://cmake.org/) 3.22 and a C++ compiler that supports the C++23 language standard. It has been tested with the Microsoft C/C++ Compiler Version 19.37.32822 on Windows and Clang 17 on Ubuntu 22.04.3 LTS. This project utilizes [CMake Presets](https://cmake.org/cmake/help/v3.22/manual/cmake-presets.7.html) to facilitate configuration, building, and testing with [ninja](https://ninja-build.org/) as a build generator.

### Package Management
This project uses [`vcpkg`](https://vcpkg.io) to manage external dependencies.  To get started, run `git submodule update --init` to clone `vcpkg` as a git submodule. `vcpkg` can then be initialized by running `.\vcpkg\bootstrap-vcpk.bat` on Windows or `./vcpkg/bootstrap-vcpkg.sh` on Linux. Upon completion, CMake will integrate with `vcpkg` to download, compile, and link external libraries specified in the [vcpkg.json](vcpkg.json) manifest when building the project.

### Address Sanitizer
This project enables [Address Sanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) (ASan) for debug builds. On Linux, this should already be available when using a modern version of GCC or Clang with C++23 support. On Windows, ASan needs to be installed separately which is documented [here](https://learn.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-170#install-addresssanitizer).

## Build

The simplest way to build the project is to use an IDE with CMake integration. Alternatively, the project can be built from the command line using CMake presets. To use the `x64-release` preset, run:

	cmake --preset x64-release
	cmake --build --preset x64-release

A list of available configuration and build presets can be displayed by running  `cmake --list-presets` and `cmake --build --list-presets` respectively. Note that on Windows, `cl` and `ninja` are expected to be available in your environment path which are available by default when using the Developer Command Prompt for Visual Studio.

## Test

This project uses [Catch2](https://github.com/catchorg/Catch2) for unit testing which can be run after building the project with [CTest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html). To use the `x64-release` preset, run:

	ctest --preset x64-release

To see what test presets are available, run `ctest --list-presets`.  Alternatively, tests can be run from the separate `tests` executable which is built with the project.

## Contribute

### Code Style
This project follows the  [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with the following exceptions:
 1. 120-character limit per line.
 2. C++ files are named with the `.cpp` file extension.
 3. Exceptions are allowed.
 4. Forward declarations are allowed.
 5. Static variables are allowed when their scope is restricted to a single translation unit.

Code style is enforced across platforms using [clang-format](https://clang.llvm.org/docs/ClangFormatStyleOptions.html).

### Git Commits
To promote descriptive commits, this project uses a commit message template which should be configured by running

	git config commit.template .gitmessage

 The template includes the following sections:
1. A short-one line summary using the [semantic commit messages](https://gist.github.com/joshbuchea/6f47e86d2510bce28f8e7f42ae84c716) format.
2. A description of the problem being addressed.
3. A description of the solution that solves the problem.
4. A description of how the solution was tested.
5. Links to relevant technical documentation for additional context.
6. References to GitHub issues related to or closed by this commit.

### Pull Requests
To ensure a high standard of code quality, all changes merged into main require a pull request with at least one approver. This project also enforces the standard of using a linear commit history which necessitates using [`git rebase`](https://git-scm.com/docs/git-rebase) when merging. Squashing is also prohibited to ensure documentation in each git commit is preserved.
