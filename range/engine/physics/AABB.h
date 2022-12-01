#pragma once
#include "vec3d.h"
#include "../graphics/mesh.h"

#include <vector>

// Axis Aligned Bounding Box
struct AABB {
	V3 min = V3(999, 999, 999);
	V3 max = V3(-999, -999, -999);
	V3 center = V3(0, 0, 0);

	AABB(Mesh* mesh, V3& pos);
};