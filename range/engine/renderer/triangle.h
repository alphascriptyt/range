#pragma once

#include "vec3D.h"
#include "colour.h"

class Triangle {
public:
	// store triangle vertices
	V3 v1 = V3(0, 0, 0);
	V3 v2 = V3(0, 0, 0);
	V3 v3 = V3(0, 0, 0);

	Colour colour = COLOUR::WHITE;

	// constructor to accept vertices as arguments
	Triangle(V3& _v1, V3& _v2, V3& _v3);

	// constructor to accept vertices and a colour as arguments
	Triangle(V3& _v1, V3& _v2, V3& _v3, Colour& _colour);
};
