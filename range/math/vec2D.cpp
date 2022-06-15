#include "Vec2D.h"
#include <iostream>

// constructors
V2::V2() {
	// initialize with default values
	x = 0;
	y = 0;
}

V2::V2(float _x, float _y) {
	// set components to given values
	x = _x;
	y = _y;
}

V2::V2(float _x, float _y, float _w) {
	// set components to given values
	x = _x;
	y = _y;
	w = _w;
}

// vector methods
void V2::print() {
	std::cout << x << " " << y << std::endl;
}

// vector functions
V2 vectorAdd(V2& v1, V2& v2) {
	return V2(
		v1.x + v2.x,
		v1.y + v2.y
	);
}

void vectorAddTo(V2& v1, V2& v2) {
	// add to the reference rather than returning new
	v1.x += v2.x;
	v1.y += v2.y;
}

V2 vectorSub(V2& v1, V2& v2) {
	return V2(
		v1.x - v2.x,
		v1.y - v2.y
	);
}