#include "collision.h"

#include <iostream>
bool pointCollidesWithAABB(V3& point, AABB& box) {

	/*
	This is as simple as calculating the distance between the camera position and the bounding volume, 
	if the distance is less than some distance stop the camera from entering the volume (do collision response).
	
	*/

	float squared_distance = 0.0f;

	if (point.x < box.min.x) {
		squared_distance += (box.min.x - point.x) * (box.min.x - point.x);
	}
	else if (point.x > box.max.x) {
		squared_distance += (point.x - box.max.x) * (point.x - box.max.x);
	}
	if (point.y < box.min.y) {
		squared_distance += (box.min.y - point.y) * (box.min.y - point.y);
	}
	else if (point.y > box.max.y) {
		squared_distance += (point.y - box.max.y) * (point.y - box.max.y);
	}
	if (point.z < box.min.z) {
		squared_distance += (box.min.z - point.z) * (box.min.z - point.z);
	}
	else if (point.z > box.max.z) {
		squared_distance += (point.z - box.max.z) * (point.z - box.max.z);
	}

	float temp = 1.0f; 
	
	if (squared_distance < temp) {
		return true;
	}
	return false;
}