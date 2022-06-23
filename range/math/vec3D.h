#pragma once

// 3D vector class
class V3 {
public:
	// components of the vector
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1; // 4th component for matrix multiplication

	// constructors
	V3();								// default sets x,y,z to {0,0,0}
	V3(float _x, float _y, float _z);	// sets x,y,z to given values
	
	// methods
	float size();			// return the size of the vector
	float sizeSquared();	// return the size squared of the vector (more performance friendly)
	void normalize();		// normalize the vector (doesn't include w)
	void makePositive();	// make the vector positive
	void print();			// easily print vector for debugging

};