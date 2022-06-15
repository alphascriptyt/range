#pragma once

// 2D vector class
class V2 {
public:
	// components of the vector
	float x = 0;
	float y = 0;
	float w = 0;

	// constructors
	V2();									// default sets x,y to {0,0}
	V2(float _x, float _y);				// sets x,y to given values
	V2(float _x, float _y, float _w);	// sets x,y,w to given values

	// methods
	void print(); // easily print vector for debugging
};

// vector related methods
V2 vectorAdd(V2& v1, V2& v2);
void vectorAddTo(V2& v1, V2& v2);
V2 vectorSub(V2& v1, V2& v2);
