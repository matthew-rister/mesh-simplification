cmake_minimum_required(VERSION 3.26)

project(VkRender VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(EnableAllWarningsAsErrors INTERFACE)
add_library(EnableAsanIfDebug INTERFACE)

if (MSVC)
    target_compile_options(EnableAllWarningsAsErrors INTERFACE /W4 /WX)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        target_compile_options(EnableAsanIfDebug INTERFACE /fsanitize=address)
    endif()
else()
    target_compile_options(EnableAllWarningsAsErrors INTERFACE -Wall -Werror -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        target_compile_options(EnableAsanIfDebug INTERFACE -fsanitize=address)
        target_link_options(EnableAsanIfDebug INTERFACE -fsanitize=address)
    endif()
endif()

add_subdirectory(src)
add_subdirectory(tests)
