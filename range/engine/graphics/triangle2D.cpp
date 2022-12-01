#include "triangle2D.h"

// constructors
Triangle2D::Triangle2D(V2& _v1, V2& _v2, V2& _v3) {
	// set given vertices
	v1 = _v1;
	v2 = _v2;
	v3 = _v3;
}

Triangle2D::Triangle2D(V2& _v1, V2& _v2, V2& _v3, Colour& _colour) {
	// set given vertices
	v1 = _v1;
	v2 = _v2;
	v3 = _v3;

	// set given colour
	colour = _colour;
}
