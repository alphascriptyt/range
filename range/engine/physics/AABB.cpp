#include "AABB.h"

// constructor
AABB::AABB(Mesh* mesh) {
	// find the minimum and maximum points from the vertices
	for (int i = 0; i < mesh->vertices.size(); ++i) {
		// find the maximum points
		if (mesh->vertices[i].x > max.x) {
			max.x = (mesh->vertices[i].x * mesh->size.x) + mesh->pos.x;
		}
		if (mesh->vertices[i].y > max.y) {
			max.y = (mesh->vertices[i].y * mesh->size.y) + mesh->pos.y;
		}
		if (mesh->vertices[i].z > max.z) {
			max.z = (mesh->vertices[i].z * mesh->size.z) + mesh->pos.z;
		}

		// find the minimum points
		if (mesh->vertices[i].x < min.x) {
			min.x = (mesh->vertices[i].x * mesh->size.x) + mesh->pos.x;
		}
		if (mesh->vertices[i].y < min.y) {
			min.y = (mesh->vertices[i].y * mesh->size.y) + mesh->pos.y;
		}
		if (mesh->vertices[i].z < min.z) {
			min.z = (mesh->vertices[i].z * mesh->size.z) + mesh->pos.z;
		}
	}
}