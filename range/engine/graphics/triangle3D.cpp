#include "Triangle3D.h"

// constructors
Triangle3D::Triangle3D(V3& _v1, V3& _v2, V3& _v3) {
	// set given vertices
	v1 = _v1;
	v2 = _v2;
	v3 = _v3;
}

Triangle3D::Triangle3D(V3& _v1, V3& _v2, V3& _v3, Colour& _colour) {
	// set given vertices
	v1 = _v1;
	v2 = _v2;
	v3 = _v3;

	// set given colour
	colour = _colour;
}
