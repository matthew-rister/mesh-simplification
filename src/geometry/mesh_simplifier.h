#ifndef SRC_GEOMETRY_MESH_SIMPLIFIER_H_
#define SRC_GEOMETRY_MESH_SIMPLIFIER_H_

namespace gfx {
class Device;
class Mesh;

namespace mesh {

Mesh Simplify(const Device& device, const Mesh& mesh, float rate);

}  // namespace mesh
}  // namespace gfx

#endif  // SRC_GEOMETRY_MESH_SIMPLIFIER_H_
