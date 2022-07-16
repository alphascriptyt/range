#pragma once
#include "vec3d.h"
#include "../renderer/mesh.h"

#include <vector>

// Axis Aligned Bounding Box
struct AABB {
	V3 min = V3(999, 999, 999);
	V3 max = V3(-999, -999, -999);

	AABB(Mesh* mesh);
};