#pragma once
#include "AABB.h"
#include "vec3d.h"

#include <vector>

// everything to do with collision detection goes here
bool pointCollidesWithAABB(V3& point, AABB& box);