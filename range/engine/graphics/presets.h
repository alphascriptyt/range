#pragma once
#include "vec3d.h"

#include <vector>

// a few preset shapes to define a mesh from

// define the cube preset properties
namespace Cube {
	extern std::vector<V3> vertices;
	extern std::vector<std::vector<int>> faces;
}