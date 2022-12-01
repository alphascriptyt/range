#pragma once

#include "vec2D.h"
#include "colour.h"

class Triangle2D {
public:
	// store triangle vertices
	V2 v1 = V2(0, 0, 0);
	V2 v2 = V2(0, 0, 0);
	V2 v3 = V2(0, 0, 0);

	Colour colour = COLOUR::WHITE;

	// constructor to accept vertices as arguments
	Triangle2D(V2& _v1, V2& _v2, V2& _v3);

	// constructor to accept vertices and a colour as arguments
	Triangle2D(V2& _v1, V2& _v2, V2& _v3, Colour& _colour);
};
