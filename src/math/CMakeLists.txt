add_library(math INTERFACE spherical_coordinates.h)

find_package(glm CONFIG REQUIRED)

target_link_libraries(math INTERFACE glm::glm)

target_compile_definitions(math INTERFACE GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
                                          GLM_FORCE_XYZW_ONLY)

target_include_directories(math INTERFACE ${CMAKE_SOURCE_DIR}/src)
