add_library(math STATIC spherical_coordinates.cpp)

find_package(glm CONFIG REQUIRED)

target_link_libraries(math PUBLIC glm::glm)
target_compile_definitions(math PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES GLM_FORCE_XYZW_ONLY)
target_include_directories(math PUBLIC include)
