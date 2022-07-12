#include "vec3D.h"
#include "mat4D.h"
#include <iostream>

// constructors
V3::V3() {
	// initialize with default values
	x = 0;
	y = 0;
	z = 0;
}

V3::V3(float _x, float _y, float _z) {
	// set components to given values
	x = _x;
	y = _y;
	z = _z;
}

// vector methods
float V3::size() {
	return sqrtf(x * x + y * y + z * z);
}

float V3::sizeSquared() {
	return x * x + y * y + z * z;
}

void V3::normalize() {
	// make the vector's size 1
	float magnitude = size();
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
}

void V3::makePositive() {
	// make every vector component positive
	x = abs(x);
	y = abs(y);
	z = abs(z);
}

void V3::print() {
	std::cout << x << " " << y << " " << z << " " << w << std::endl;
}

