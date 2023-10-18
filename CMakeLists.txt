cmake_minimum_required(VERSION 3.26)
project(vkrender VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(enable_all_warnings_as_errors INTERFACE)
add_library(enable_asan_if_debug INTERFACE)

if (MSVC)
    target_compile_options(enable_all_warnings_as_errors INTERFACE /W4 /WX)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        target_compile_options(enable_asan_if_debug INTERFACE /fsanitize=address)
    endif()
else()
    target_compile_options(enable_all_warnings_as_errors INTERFACE -Wall -Werror -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        target_compile_options(enable_asan_if_debug INTERFACE -fsanitize=address)
        target_link_options(enable_asan_if_debug INTERFACE -fsanitize=address)
    endif()
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
