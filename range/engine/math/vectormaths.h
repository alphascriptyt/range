#pragma once
#include "mat4D.h"
#include "vec3D.h"
#include "plane.h"

// TODO: WRITE COMMENTS
// TODO: inline everything?

#include <cmath>


// vector functions
inline V3 vectorCrossProduct(V3& v1, V3& v2) {
	// calculate the cross product of two vectors
	return V3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

inline float vectorDotProduct(V3& v1, V3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline V3 vectorIntersectPlane(V3& v1, V3& v2, Plane& plane) {
	// standard algorithm for calculating the intersection between a point and plane
	float plane_d = -vectorDotProduct(plane.normal, plane.point);

	float ad = vectorDotProduct(v1, plane.normal);
	float bd = vectorDotProduct(v2, plane.normal);
	V3 line_start_to_end = v2 - v1;

	float t = (-plane_d - ad) / vectorDotProduct(line_start_to_end, plane.normal);

	V3 line_to_intersect = line_start_to_end * t;

	return v1 + line_to_intersect;
}

inline V3 multiplyBy4x1(M4& m, V3& v) {
	V3 out;

	out.x = (m[0][0] * v.x) + (m[0][1] * v.y) + (m[0][2] * v.z) + (m[0][3] * v.w);
	out.y = (m[1][0] * v.x) + (m[1][1] * v.y) + (m[1][2] * v.z) + (m[1][3] * v.w);
	out.z = (m[2][0] * v.x) + (m[2][1] * v.y) + (m[2][2] * v.z) + (m[2][3] * v.w);
	out.w = (m[3][0] * v.x) + (m[3][1] * v.y) + (m[3][2] * v.z) + (m[3][3] * v.w);

	return out;
}

inline void rotateV3(V3& v, float pitch, float yaw) {
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

inline float findSignedDistance(V3& v, Plane& plane) {
	// method for finding the signed distance 
	// between a plane and a vertex

	// find the vector between the plane point and vertex
	V3 between = v - plane.point;

	// return the signed distance that the between vector goes
	// in the direction of the plane's normal
	return vectorDotProduct(plane.normal, between);
}

inline bool isFrontFacing(V3& v1, V3& v2, V3& v3) {
	// algorithm to check whether a face should be drawn
	// or if it is on the backside of the shape and therefore
	// cannot be seen
	V3 vs1 = v2 - v1;
	V3 vs2 = v3 - v1;
	V3 n = vectorCrossProduct(vs1, vs2);

	return vectorDotProduct(v1, n) <= 0;
}