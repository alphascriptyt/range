#pragma once

#include "vec3D.h"
#include "colour.h"

class Triangle3D {
public:
	// store triangle propeties
	V3 v1;
	V3 v2;
	V3 v3;
	V3 normal;

	Colour colour = COLOUR::WHITE;
	Colour colourV1 = COLOUR::WHITE;
	Colour colourV2 = COLOUR::WHITE;
	Colour colourV3 = COLOUR::WHITE;

	// constructor to accept vertices as arguments
	Triangle3D(V3& _v1, V3& _v2, V3& _v3);

	// constructor to accept vertices and a colour as arguments
	Triangle3D(V3& _v1, V3& _v2, V3& _v3, Colour& _colour);
};
