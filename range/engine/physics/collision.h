#pragma once
#include "AABB.h"
#include "vec3d.h"

#include <vector>

// everything to do with collision detection goes here
bool pointCollidesWithAABB(V3& point, AABB& box, V3& intersection, float& dist);
bool doAABBCollide(AABB& a, AABB& b);
V3 rayCollidesWithAABB(AABB& box, V3& ray_origin, V3& ray_direction);