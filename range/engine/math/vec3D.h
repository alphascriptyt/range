#pragma once

#include "mat4D.h"

// 3D vector class
class V3 {
public:
	// components of the vector
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1; // 4th component for matrix multiplication
	float id = 0;

	// constructors
	V3();								// default sets x,y,z to {0,0,0}
	V3(float _x, float _y, float _z);	// sets x,y,z to given values
	
	// methods
	float size();			// return the size of the vector
	float sizeSquared();	// return the size squared of the vector (more performance friendly)
	void normalize();		// normalize the vector (doesn't include w)
	void makePositive();	// make the vector positive
	void print();			// easily print vector for debugging

	// operator overloading TODO: put in .cpp
	inline V3 operator+(const V3& v) {
		return V3(
			x + v.x,
			y + v.y,
			z + v.z
		);
	}
	inline V3 operator+(const float n) {
		return V3(
			x + n,
			y + n,
			z + n
		);
	}
	inline void operator+=(const V3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}
	inline void operator+=(const float n) {
		x += n;
		y += n;
		z += n;
	}

	inline V3 operator-(const V3& v) {
		return V3(
			x - v.x,
			y - v.y,
			z - v.z
		);
	}
	inline V3 operator-(const float n)
	{
		return V3(
			x - n,
			y - n,
			z - n
		);
	}
	inline void operator-=(const V3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	inline void operator-=(const float n) {
		x -= n;
		y -= n;
		z -= n;
	}

	inline V3 operator*(const V3& v) {
		return V3(
			x * v.x,
			y * v.y,
			z * v.z
		);
	}
	inline V3 operator*(const float n) {
		return V3(
			x * n,
			y * n,
			z * n
		);
	}
	inline void operator*=(const V3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}
	inline void operator*=(const float n) {
		x *= n;
		y *= n;
		z *= n;
	}

	inline V3 operator/(const V3& v) {
		return V3(
			x / v.x,
			y / v.y,
			z / v.z
		);
	}
	inline V3 operator/(const float n) {
		return V3(
			x / n,
			y / n,
			z / n
		);
	}
	inline void operator/=(const V3& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}
	inline void operator/=(const float n) {
		x /= n;
		y /= n;
		z /= n;
	}

	inline void operator=(const float n) {
		x = n;
		y = n;
		z = n;
	}
	inline bool operator==(const V3& v) {
		return x == v.x && y == v.y && z == v.z;
	}

	inline V3 operator*(M4& m) {
		V3 out;
		out.x = (m[0][0] * x) + (m[1][0] * y) + (m[2][0] * z) + (m[3][0] * w);
		out.y = (m[0][1] * x) + (m[1][1] * y) + (m[2][1] * z) + (m[3][1] * w);
		out.z = (m[0][2] * x) + (m[1][2] * y) + (m[2][2] * z) + (m[3][2] * w);
		out.w = (m[0][3] * x) + (m[1][3] * y) + (m[2][3] * z) + (m[3][3] * w);

		return out;
	}
};

