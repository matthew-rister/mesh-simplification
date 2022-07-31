# Mesh Simplification

In computer graphics, working with highly complex models can degrade rendering performance. One technique to mitigate this situation is to use simplified models by reducing the number of triangles in a polygon mesh. This project presents an efficient algorithm to achieve this based on a research paper by Garland-Heckbert titled [Surface Simplification Using Quadric Error Metrics](https://github.com/matthew-rister/mesh-simplification/blob/main/MeshSimplification/docs/surface_simplification.pdf).

The central idea of the algorithm is to iteratively remove edges in the mesh through a process known as [edge contraction](https://en.wikipedia.org/wiki/Edge_contraction) which merges the vertices at an edge's endpoints into a new vertex that optimally preserves the original shape of the mesh. This vertex position can be solved for analytically by minimizing the squared distance of each adjacent triangle's plane to its new position after edge contraction. With this error metric, edges can be efficiently processed using a priority queue to remove edges with the lowest cost until the mesh has been sufficiently simplified. To facilitate the implementation of this algorithm, a data structure known as a [half-edge mesh](https://github.com/matthew-rister/mesh-simplification/blob/main/MeshSimplification/geometry/half_edge_mesh.h) is employed to efficiently traverse and modify edges in the mesh.

## Results

The following GIF presents a real-time demonstration of successive applications of mesh simplification on a polygon mesh consisting of nearly 70,000 triangles. At each iteration, the number of triangles is reduced by 50% eventually reducing to a mesh consisting of only 1,086 triangles (a 98.5% reduction). Observe that although fidelity is reduced, the mesh retains an overall high-quality appearance that nicely approximates the original shape of the mesh.

![](https://github.com/matthew-rister/mesh-simplification/blob/main/MeshSimplification.gif)

## How To Run

This project was implemented using Visual Studio in C++20 and OpenGL 4.6. To build it, you will need a version of Visual Studio that supports the latest language standard (e.g., 16.10.0). Additionally, this project uses [vcpkg](https://vcpkg.io/en/index.html) to manage 3rd party dependencies. To get started, clone the [vcpkg repository](https://github.com/microsoft/vcpkg) on Github and run `bootstrap-vcpkg.bat` followed by `vcpkg integrate install`. Upon completion, you should be able to build and run the project which will install dependencies from the [vcpkg.json](https://github.com/matthew-rister/mesh-simplification/blob/main/vcpkg.json) package manifest on first build.

## Usage

Once the program is running, the mesh can be simplified by pressing the `S` key. Additionally, the mesh can be translated or rotated about an arbitrary axis by left or right clicking on the mouse and dragging the cursor across the screen. Lastly, the mesh can be uniformly scaled using the mouse scroll wheel. 
