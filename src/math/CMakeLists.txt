add_library(math STATIC)

target_sources(
  math
  PUBLIC FILE_SET HEADERS BASE_DIRS ${SRC_DIR} FILES spherical_coordinates.h
  PRIVATE spherical_coordinates.cpp)

find_package(glm CONFIG REQUIRED)

target_link_libraries(math PUBLIC glm::glm)
target_compile_definitions(math PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES GLM_FORCE_XYZW_ONLY)
