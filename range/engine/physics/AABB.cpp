#include "AABB.h"

// constructor
AABB::AABB(Mesh* mesh) {
	V3 vertex_world;
	// find the minimum and maximum points from the vertices
	for (int i = 0; i < mesh->vertices.size(); ++i) {
		// calculate the world position of the vertex
		vertex_world = (mesh->vertices[i] * mesh->size) + mesh->pos;

		// find the maximum points
		if (vertex_world.x > max.x) {
			max.x = vertex_world.x;
		}
		if (vertex_world.y > max.y) {
			max.y = vertex_world.y;
		}
		if (vertex_world.z > max.z) {
			max.z = vertex_world.z;
		}

		// find the minimum points
		if (vertex_world.x < min.x) {
			min.x = vertex_world.x;
		}
		if (vertex_world.y < min.y) {
			min.y = vertex_world.y;
		}
		if (vertex_world.z < min.z) {
			min.z = vertex_world.z;
		}
	}
}