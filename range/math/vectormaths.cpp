#include "vectormaths.h"
#include <cmath>

// vector functions
V3 vectorAdd(V3& v1, V3& v2) {
	return V3(
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z
	);
}

void vectorAddTo(V3& v1, V3& v2) {
	// add to the reference rather than returning new
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

V3 vectorSub(V3& v1, V3& v2) {
	return V3(
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z
	);
}

V3 vectorMult(V3& v1, V3& v2) {
	return V3(
		v1.x * v2.x,
		v1.y * v2.y,
		v1.z * v2.z
	);
}

void vectorMultBy(V3& v1, V3& v2) {
	// multiply the reference rather than returning new
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
}

V3 vectorCrossProduct(V3& v1, V3& v2) {
	// calculate the cross product of two vectors
	return V3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

float vectorDotProduct(V3& v1, V3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
#include <iostream>
V3 vectorIntersectPlane(V3& v1, V3& v2, V3& plane_normal, V3& plane_point) {
	// standard algorithm for calculating the intersection between a point and plane
	float plane_d = -vectorDotProduct(plane_normal, plane_point);

	float ad = vectorDotProduct(v1, plane_normal);
	float bd = vectorDotProduct(v2, plane_normal);
	V3 line_start_to_end = vectorSub(v2, v1);



	float t = (-plane_d - ad) / vectorDotProduct(line_start_to_end, plane_normal);
	
	V3 line_to_intersect = line_start_to_end;
	line_to_intersect.x *= t;
	line_to_intersect.y *= t;
	line_to_intersect.z *= t;

	return vectorAdd(v1, line_to_intersect);
}

void rotateV3(V3& v, float pitch, float yaw) {
	// precalculate trig to avoid multiple calculations
	float sin_pitch = sin(pitch);
	float sin_yaw = sin(yaw);
	float cos_pitch = cos(pitch);
	float cos_yaw = cos(yaw);

	// setup rotation matrix (combined X and Y axis)
	M4 rotation_matrix;
	rotation_matrix.setRow(0, cos_yaw, 0, -sin_yaw, 0);
	rotation_matrix.setRow(1, sin_pitch * sin_yaw, cos_pitch, sin_pitch * cos_yaw, 0);
	rotation_matrix.setRow(2, cos_pitch * sin_yaw, -sin_pitch, cos_pitch * cos_yaw, 0);
	rotation_matrix.setRow(3, 0, 0, 0, 1);

	// actually rotate the vertex
	v = multiplyBy4x1(rotation_matrix, v);
}

/*
M4 multiplyByM4(M4& m2) {
	M4 m; // out matrix
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			m[r][c] = elements[r][0] * m[0][c] + elements[r][1] * m[1][c] + elements[r][2] * m[2][c] + elements[r][3] * m[3][c];
		}
	}
	return m;
}
*/

V3 multiplyBy4x1(M4& m, V3& v) {
	V3 out;

	out.x = (m[0][0] * v.x) + (m[0][1] * v.y) + (m[0][2] * v.z) + (m[0][3] * v.w);
	out.y = (m[1][0] * v.x) + (m[1][1] * v.y) + (m[1][2] * v.z) + (m[1][3] * v.w);
	out.z = (m[2][0] * v.x) + (m[2][1] * v.y) + (m[2][2] * v.z) + (m[2][3] * v.w);
	out.w = (m[3][0] * v.x) + (m[3][1] * v.y) + (m[3][2] * v.z) + (m[3][3] * v.w);

	return out;
}