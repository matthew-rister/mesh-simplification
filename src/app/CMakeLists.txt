add_executable(mesh_simplification)

target_sources(
  mesh_simplification
  PUBLIC FILE_SET HEADERS BASE_DIRS ${SRC_DIR} FILES app.h
  PRIVATE main.cpp app.cpp)

target_link_libraries(mesh_simplification PRIVATE geometry graphics)

set(ASSETS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(ASSETS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/assets)

add_custom_target(
  create_symlink ALL
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${ASSETS_SOURCE_DIR} ${ASSETS_BINARY_DIR}
  COMMENT "Creating assets symlink from ${ASSETS_SOURCE_DIR} to ${ASSETS_BINARY_DIR}")
