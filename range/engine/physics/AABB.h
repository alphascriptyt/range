#pragma once
#include "vec3d.h"
#include "../renderer/mesh.h"

#include <vector>

// Axis Aligned Bounding Box
struct AABB {
	V3 min;
	V3 max;

	AABB(Mesh* mesh);
};