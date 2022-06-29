#pragma once

#include "vec3D.h"

// class for storing a 3D plane
struct Plane {
	V3 normal;
	V3 point;

	Plane(V3& n, V3& p);
};