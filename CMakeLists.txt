cmake_minimum_required(VERSION 3.26)
project(vkrender VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dbgasan INTERFACE)
add_library(warnings INTERFACE)

if(MSVC)
  target_compile_options(warnings INTERFACE /W4 /WX)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(dbgasan INTERFACE /fsanitize=address)
  endif()
else()
  target_compile_options(warnings INTERFACE -Wall -Werror -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_options(dbgasan INTERFACE -fsanitize=address)
    target_link_options(dbgasan INTERFACE -fsanitize=address)
  endif()
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
