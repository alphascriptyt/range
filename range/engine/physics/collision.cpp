#include "collision.h"

#include <iostream>
bool pointCollidesWithAABB(V3& point, AABB& box, V3& intersection, float& dist) {

	/*
	This is as simple as calculating the distance between the camera position and the bounding volume, 
	if the distance is less than some distance stop the camera from entering the volume (do collision response).
	*/

	float squared_distance = 0.0f;

	if (point.x < box.min.x) {
		squared_distance += (box.min.x - point.x) * (box.min.x - point.x);
		intersection.x = box.min.x;
	}
	else if (point.x > box.max.x) {
		squared_distance += (point.x - box.max.x) * (point.x - box.max.x);
		intersection.x = box.max.x;
	}
	if (point.y < box.min.y) {
		squared_distance += (box.min.y - point.y) * (box.min.y - point.y);
		intersection.y = box.min.y;
	}
	else if (point.y > box.max.y) {
		squared_distance += (point.y - box.max.y) * (point.y - box.max.y);
		intersection.y = box.max.y;
	}
	if (point.z < box.min.z) {
		squared_distance += (box.min.z - point.z) * (box.min.z - point.z);
		intersection.z = box.min.z;
	}
	else if (point.z > box.max.z) {
		squared_distance += (point.z - box.max.z) * (point.z - box.max.z);
		intersection.z = box.max.z;
	}

	float temp = 0.25; 

	if (squared_distance < temp) {
		dist = squared_distance;
		return true;
	}
	return false;
}