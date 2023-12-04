cmake_minimum_required(VERSION 3.27)
project(mesh_simplification VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# these interface targets are used for privately sharing common compiler configurations
add_library(asan INTERFACE)
add_library(warnings INTERFACE)

if(MSVC)
  target_compile_options(warnings INTERFACE /W4 /WX)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(asan INTERFACE /fsanitize=address)
  endif()
else()
  target_compile_options(warnings INTERFACE -Wall -Werror -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(asan INTERFACE -fsanitize=address)
    target_link_options(asan INTERFACE -fsanitize=address)
  endif()
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
