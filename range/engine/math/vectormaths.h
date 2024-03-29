#pragma once
#include "mat4D.h"
#include "vec3D.h"
#include "plane.h"

// TODO: WRITE COMMENTS
// TODO: inline everything?

#include <cmath>

#include <iostream>
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

inline V3 vectorIntersectPlane(V3& v1, V3& v2, Plane& plane, float& t) {
	// standard algorithm for calculating the intersection between a point and plane
	float plane_d = -vectorDotProduct(plane.normal, plane.point);

	float ad = vectorDotProduct(v1, plane.normal);
	float bd = vectorDotProduct(v2, plane.normal);
	V3 line_start_to_end = v2 - v1;

	//t = (-plane_d - ad) / vectorDotProduct(line_start_to_end, plane.normal);
	t = (-plane_d - ad) / (bd - ad);

	//std::cout << "T: " << t << std::endl;

	V3 line_to_intersect = line_start_to_end * t;

	V3 nv = v1 + line_to_intersect;
	//nv.c = v2.c;
	nv.c = v1.c + (v2.c - v1.c) * t;
	//nv.c = COLOUR::WHITE;
	
	//std::cout << "col: "; nv.c.print();

	return nv;

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

inline M4 makeLookAtMatrix(const V3& position, const V3& direction) {
	V3 pos = position;

	V3 zaxis = direction;

	// get positive right axis vector
	V3 world_up(0, 1, 0);
	V3 xaxis = vectorCrossProduct(world_up, zaxis);
	xaxis.normalize();

	// calculate camera up vector
	V3 yaxis = vectorCrossProduct(zaxis, xaxis);

	//yaxis.normalize();
	//yaxis.print("yaxis: ");
	
	// create translation and rotation matrix
	/*
	M4 translation;
	translation.makeIdentity();
	translation[3][0] = position.x;
	translation[3][1] = position.y;
	translation[3][2] = position.z;

	M4 rotation;
	rotation.makeIdentity();
	
	rotation[0][0] = xaxis.x;
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x;
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x;
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;

	return translation * rotation; // TODO: switching order stops the y from being based off the x/z but breaks rotaiton
	*/

	M4 transform;
	transform.makeIdentity();

	transform[0][0] = xaxis.x;
	transform[1][0] = xaxis.y;
	transform[2][0] = xaxis.z;
	transform[0][1] = yaxis.x;
	transform[1][1] = yaxis.y;
	transform[2][1] = yaxis.z;
	transform[0][2] = zaxis.x;
	transform[1][2] = zaxis.y;
	transform[2][2] = zaxis.z; 
	transform[3][0] = vectorDotProduct(xaxis, pos);
	transform[3][1] = vectorDotProduct(yaxis, pos);
	transform[3][2] = vectorDotProduct(zaxis, pos);

	// y = x * yaxis.x + y * yaxis.y + z * yaxis.z + vectorDotProduct(yaxis, pos)

	return transform;
}

inline void calculateBarycentric(V3 a, V3 b, V3 c, float& u, float& v, float& w) {
	
	// P = uA + vB + wC	
	// TODO: need to understand these more
	// https://www.youtube.com/watch?v=y2sNnI4NrDc&t=9s

}