add_library(geometry STATIC)

target_sources(
  geometry
  # cmake-format: off
  PUBLIC FILE_SET HEADERS
         BASE_DIRS ${SRC_DIR}
         FILES face.h
               half_edge.h
               half_edge_mesh.h
               mesh_simplifier.h
               vertex.h
  # cmake-format: on
  PRIVATE face.cpp half_edge_mesh.cpp mesh_simplifier.cpp)

find_package(glm CONFIG REQUIRED)

target_link_libraries(geometry PUBLIC glm::glm graphics)
target_compile_definitions(geometry PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES GLM_FORCE_XYZW_ONLY)
