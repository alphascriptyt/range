#include "physics.h"
#include "../scene.h"
#include "vectormaths.h"
#include <iostream>
#include "collision.h"
#include "AABB.h"
#include "physicsdata.h"
#include "../graphics/triangle3D.h"

// This function solves the quadratic eqation "At^2 + Bt + C = 0" and is found in Kasper Fauerby's paper on collision detection and response
bool getLowestRoot(float a, float b, float c, float maxR, float* root)
{
	// Check if a solution exists
	float determinant = b * b - 4.0f * a * c;
	// If determinant is negative it means no solutions.
	if (determinant < 0.0f) return false;
	// calculate the two roots: (if determinant == 0 then
	// x1==x2 but lets disregard that slight optimization)
	float sqrtD = sqrt(determinant);
	float r1 = (-b - sqrtD) / (2 * a);
	float r2 = (-b + sqrtD) / (2 * a);
	// Sort so x1 <= x2
	if (r1 > r2) {
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	// Get lowest root:
	if (r1 > 0 && r1 < maxR) {
		*root = r1;
		return true;
	}
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < maxR) {
		*root = r2;
		return true;
	}

	// No (valid) solutions
	return false;
}

bool checkPointInTriangle2(V3& point, V3& a, V3& b, V3& c) {
	// https://blackpawn.com/texts/pointinpoly/
	// https://www.youtube.com/watch?v=HYAgJN3x4GA
	// Compute vectors        
	V3 v0 = c - a;
	V3 v1 = b - a;
	V3 v2 = point - a;

	// Compute dot products
	float dot00 = vectorDotProduct(v0, v0);
	float dot01 = vectorDotProduct(v0, v1);
	float dot02 = vectorDotProduct(v0, v2);
	float dot11 = vectorDotProduct(v1, v1);
	float dot12 = vectorDotProduct(v1, v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}

void testVertexAndSphere(V3& v, V3& position, V3& velocity, float a, float& t, bool& colliding_with_tri, V3& collision) {
	// P0 - Collision test with sphere and v1
	V3 pos_to_vertex = position - v;
	float b = 2.0f * (vectorDotProduct(velocity, pos_to_vertex));
	V3 edge = v - position;
	float c = edge.sizeSquared() - 1.0f;
	float newT;
	if (getLowestRoot(a, b, c, t, &newT)) {    // Check if the equation can be solved
		// If the equation was solved, we can set a couple things. First we set t (distance
		// down velocity vector the sphere first collides with vertex) to the temporary newT,
		// Then we set collidingWithTri to be true so we know there was for sure a collision
		// with the triangle, then we set the exact point the sphere collides with the triangle,
		// which is the position of the vertex it collides with
		t = newT;
		colliding_with_tri = true;
		collision = v;
	}
}

void testEdgeAndSphere(V3& v1, V3& v2, V3& position, V3& velocity, float velocity_length_squared, float& t, bool& colliding_with_tri, V3& collision) {
	// Edge (v1, v2):
	V3 edge = v2 - v1;
	V3 spherePositionToVertex = v1 - position;
	float edgeLengthSquared = edge.sizeSquared();

	float edgeDotVelocity = vectorDotProduct(edge, velocity);
	float edgeDotSpherePositionToVertex = vectorDotProduct(edge, spherePositionToVertex);
	float spherePositionToVertexLengthSquared = spherePositionToVertex.sizeSquared();

	// Equation parameters
	float a = edgeLengthSquared * -velocity_length_squared + (edgeDotVelocity * edgeDotVelocity);
	float b = edgeLengthSquared * (2.0f * vectorDotProduct(velocity, spherePositionToVertex)) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
	float c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

	float newT;
	// We start by finding if the swept sphere collides with the edges "infinite line"
	if (getLowestRoot(a, b, c, t, &newT)) {
		// Now we check to see if the collision happened between the two vertices that make up this edge
		// We can calculate where on the line the collision happens by doing this:
		// f = (edge . velocity)newT - (edge . spherePositionToVertex) / edgeLength^2
		// if f is between 0 and 1, then we know the collision happened between p0 and p1
		// If the collision happened at p0, the f = 0, if the collision happened at p1 then f = 1
		float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
		if (f >= 0.0f && f <= 1.0f) {
			// If the collision with the edge happened, we set the results
			t = newT;
			colliding_with_tri = true;
			collision = v1 + edge * f;
		}
	}
}

/*
bool Physics::findCollision(Entity& entity1, Entity& entity2) {
	// convert everything to ellipsoid space
	v1 /= ellipsoid;
	v2 /= ellipsoid;
	v3 /= ellipsoid;

	V3 position = camera.physics.position / ellipsoid;
	V3 velocity = camera.physics.velocity / ellipsoid * dt;

	// calculate the triangle plane
	V3 edge1 = v2 - v1;
	V3 edge2 = v3 - v1;
	V3 normal = vectorCrossProduct(edge1, edge2);
	normal.normalize();

	Plane tri_plane = Plane(normal, v1);

	// find the velocity direction
	V3 normalised_velocity = velocity;
	normalised_velocity.normalize();

	// does the 'sphere' collide with the triangle

	// does the normal gone in the same direction as the velocity?
	float dot_product = vectorDotProduct(normal, normalised_velocity);

	// ignore backfaces as we cannot collide with them
	if (dot_product >= 0) {
		continue;
	}

	// pre-calculate values to do with the triangle plane
	float signed_dist = findSignedDistance(position, tri_plane);
	float dot_normal_velocity = vectorDotProduct(tri_plane.normal, velocity);

	bool sphere_in_plane = false;
	float t0 = 0.0f;
	float t1 = 1.0f;

	// check if velocity is perpendicular to plane NORMAL, then swept sphere is moving parallel to the plane
	if (dot_normal_velocity == 0.0f) {
		if (fabs(signed_dist) >= 1.0f) { // unit sphere radius of 1, so check if plane is outside this distance
			// sphere is not within 1 unit of plane and moving parallel to plane, so collision is impossible.
			continue;
		}
		else {
			sphere_in_plane = true;
		}
	}
	else {
		// velocity at some point will intersect with plane because both are infinitely long

		// calculate time of intersection
		t0 = (-1.0f - signed_dist) / dot_normal_velocity;
		t1 = (1.0f - signed_dist) / dot_normal_velocity;

		// ensure t0 is smaller so it is the first point where the sphere touches
		if (t0 > t1) {
			std::swap(t0, t1);
		}

		// if sphere intersects the plane outside of the velocity, then it doesn't intersect the face
		if (t0 > 1.0f || t1 < 0.0f) {
			continue;
		}

		// clamp t0 and t1
		if (t0 < 0.0) { t0 = 0.0; }
		if (t1 > 1.0) { t1 = 1.0; }
	}

	// test if the sphere is inside the triangle
	V3 collision;
	bool colliding_with_tri = false;
	float t = 1.0f;

	if (!sphere_in_plane) {
		V3 plane_intersection = position - tri_plane.normal + velocity * t0;

		if (checkPointInTriangle2(plane_intersection, v1, v2, v3)) {
			colliding_with_tri = true;
			t = t0;
			collision = plane_intersection;
		}
	}

	// sphere vertex collision test
	if (!colliding_with_tri) {
		// We will be working with the quadratic function "At^2 + Bt + C = 0" to find when (t) the "swept sphere"s center
		// is 1 unit (spheres radius) away from the vertex position. Remember the swept spheres position is actually a line defined
		// by the spheres position and velocity. t represents it's position along the velocity vector.
		// a = sphereVelocityLength * sphereVelocityLength
		// b = 2(sphereVelocity . (spherePosition - vertexPosition))    // . denotes dot product
		// c = (vertexPosition - spherePosition)^2 - 1
		// This equation allows for two solutions. One is when the sphere "first" touches the vertex, and the other is when
		// the "other" side of the sphere touches the vertex on it's way past the vertex. We need the first "touch"
		float a; // Equation Parameters

		// We can use the squared velocities length below when checking for collisions with the edges of the triangles
		// to, so to keep things clear, we won't set a directly
		float velocityLengthSquared = velocity.sizeSquared();

		// We'll start by setting 'a', since all 3 point equations use this 'a'
		a = velocityLengthSquared;

		// Collision test with sphere and v1
		testVertexAndSphere(v1, position, velocity, a, t, colliding_with_tri, collision);

		// Collision test with sphere and v2
		testVertexAndSphere(v2, position, velocity, a, t, colliding_with_tri, collision);

		// Collision test with sphere and v3
		testVertexAndSphere(v3, position, velocity, a, t, colliding_with_tri, collision);

		//////////////////////////////////////////////Sphere-Edge Collision Test//////////////////////////////////////////////
		// Even though there might have been a collision with a vertex, we will still check for a collision with an edge of the
		// triangle in case an edge was hit before the vertex. Again we will solve a quadratic equation to find where (and if)
		// the swept sphere's position is 1 unit away from the edge of the triangle. The equation parameters this time are a 
		// bit more complex: (still "Ax^2 + Bx + C = 0")
		// a = edgeLength^2 * -velocityLength^2 + (edge . velocity)^2
		// b = edgeLength^2 * 2(velocity . spherePositionToVertex) - 2((edge . velocity)(edge . spherePositionToVertex))
		// c =  edgeLength^2 * (1 - spherePositionToVertexLength^2) + (edge . spherePositionToVertex)^2
		// . denotes dot product

		// edge v1v2
		testEdgeAndSphere(v1, v2, position, velocity, velocityLengthSquared, t, colliding_with_tri, collision);

		// Edge (v2, v3):
		testEdgeAndSphere(v2, v3, position, velocity, velocityLengthSquared, t, colliding_with_tri, collision);

		// Edge (v3, v1):
		testEdgeAndSphere(v3, v1, position, velocity, velocityLengthSquared, t, colliding_with_tri, collision);
	}

	if (!colliding_with_tri) {
		continue;
	}
}
*/


void Physics::process(Camera& camera, float dt) {
	// TODO: come back to mesh vs mesh collisions, realistically there isn't much point for the game.
	//		 but i would definitely like to add it eventually.

	// update the velocity for the camera (apply gravity)
	camera.physics.updateVelocity(dt);

	for (int i = 0; i < Scene::scenes[0]->entities.size(); ++i) {
		// get the scene entity
		Entity* entity = Scene::scenes[0]->entities[i];

		// get the entity's physics data
		PhysicsData* data = entity->physics;

		// find mesh bounding box, maybe pre-calculate this and save for clipping??
		AABB aabb(entity->mesh, data->position);

		// find collisions with camera and mesh
		V3 intersection_old;
		float dist;

		// define a radius for the ellipsoid 
		V3 ellipsoid(1, 3, 1);

		// calculate intended camera destination
		V3 initial_destination = camera.physics.position + camera.physics.velocity * dt;

		// check if we're possibly going to collide with the mesh
		if (pointCollidesWithAABB(initial_destination, aabb, intersection_old, dist)) {
			Mesh* mesh = entity->mesh;

			// check each face for an actual collision with each face
			for (int i = 0; i < mesh->faces.size(); ++i) {
				// get mesh face vertices
				V3 v1 = mesh->vertices[mesh->faces[i][0]];
				V3 v2 = mesh->vertices[mesh->faces[i][1]];
				V3 v3 = mesh->vertices[mesh->faces[i][2]];

				// convert raw vertices to world coordinates
				rotateV3(v1, mesh->pitch, mesh->yaw);
				rotateV3(v2, mesh->pitch, mesh->yaw);
				rotateV3(v3, mesh->pitch, mesh->yaw);

				v1 *= mesh->size;
				v2 *= mesh->size;
				v3 *= mesh->size;

				v1 += data->position;
				v2 += data->position;
				v3 += data->position;

				// CAMERA SLIDING USING ELLIPSOID SPACE CREDITS: http://www.peroxide.dk/papers/collision/collision.pdf - Kasper Fauerby

				// convert everything to ellipsoid space
				v1 /= ellipsoid;
				v2 /= ellipsoid;
				v3 /= ellipsoid;

				V3 position = camera.physics.position / ellipsoid;
				V3 velocity = camera.physics.velocity / ellipsoid * dt;

				// calculate the triangle plane
				V3 edge1 = v2 - v1;
				V3 edge2 = v3 - v1;
				V3 normal = vectorCrossProduct(edge1, edge2);
				normal.normalize();

				Plane tri_plane = Plane(normal, v1);

				// find the velocity direction
				V3 normalised_velocity = velocity;
				normalised_velocity.normalize();

				// does the 'sphere' collide with the triangle

				// does the normal gone in the same direction as the velocity?
				float dot_product = vectorDotProduct(normal, normalised_velocity);

				// ignore backfaces as we cannot collide with them
				if (dot_product >= 0) {
					continue;
				}

				// pre-calculate values to do with the triangle plane
				float signed_dist = findSignedDistance(position, tri_plane);
				float dot_normal_velocity = vectorDotProduct(tri_plane.normal, velocity);

				bool sphere_in_plane = false;
				float t0 = 0.0f;
				float t1 = 1.0f;

				// check if velocity is perpendicular to plane NORMAL, then swept sphere is moving parallel to the plane
				if (dot_normal_velocity == 0.0f) {
					if (fabs(signed_dist) >= 1.0f) { // unit sphere radius of 1, so check if plane is outside this distance
						// sphere is not within 1 unit of plane and moving parallel to plane, so collision is impossible.
						continue;
					}
					else {
						sphere_in_plane = true;
					}
				}
				else {
					// velocity at some point will intersect with plane because both are infinitely long

					// calculate time of intersection
					t0 = (-1.0f - signed_dist) / dot_normal_velocity;
					t1 = (1.0f - signed_dist) / dot_normal_velocity;

					// ensure t0 is smaller so it is the first point where the sphere touches
					if (t0 > t1) {
						std::swap(t0, t1);
					}

					// if sphere intersects the plane outside of the velocity, then it doesn't intersect the face
					if (t0 > 1.0f || t1 < 0.0f) {
						continue;
					}

					// clamp t0 and t1
					if (t0 < 0.0) { t0 = 0.0; }
					if (t1 > 1.0) { t1 = 1.0; }
				}

				// test if the sphere is inside the triangle
				V3 collision;
				bool colliding_with_tri = false;
				float t = 1.0f;

				if (!sphere_in_plane) {
					V3 plane_intersection = position - tri_plane.normal + velocity * t0;

					if (checkPointInTriangle2(plane_intersection, v1, v2, v3)) {
						colliding_with_tri = true;
						t = t0;
						collision = plane_intersection;
					}
				}

				// sphere vertex collision test
				if (!colliding_with_tri) {
					// We will be working with the quadratic function "At^2 + Bt + C = 0" to find when (t) the "swept sphere"s center
					// is 1 unit (spheres radius) away from the vertex position. Remember the swept spheres position is actually a line defined
					// by the spheres position and velocity. t represents it's position along the velocity vector.
					// a = sphereVelocityLength * sphereVelocityLength
					// b = 2(sphereVelocity . (spherePosition - vertexPosition))    // . denotes dot product
					// c = (vertexPosition - spherePosition)^2 - 1
					// This equation allows for two solutions. One is when the sphere "first" touches the vertex, and the other is when
					// the "other" side of the sphere touches the vertex on it's way past the vertex. We need the first "touch"
					float a; // Equation Parameters

					// We can use the squared velocities length below when checking for collisions with the edges of the triangles
					// to, so to keep things clear, we won't set a directly
					float velocityLengthSquared = velocity.sizeSquared();

					// We'll start by setting 'a', since all 3 point equations use this 'a'
					a = velocityLengthSquared;

					// Collision test with sphere and v1
					testVertexAndSphere(v1, position, velocity, a, t, colliding_with_tri, collision);

					// Collision test with sphere and v2
					testVertexAndSphere(v2, position, velocity, a, t, colliding_with_tri, collision);

					// Collision test with sphere and v3
					testVertexAndSphere(v3, position, velocity, a, t, colliding_with_tri, collision);

					//////////////////////////////////////////////Sphere-Edge Collision Test//////////////////////////////////////////////
					// Even though there might have been a collision with a vertex, we will still check for a collision with an edge of the
					// triangle in case an edge was hit before the vertex. Again we will solve a quadratic equation to find where (and if)
					// the swept sphere's position is 1 unit away from the edge of the triangle. The equation parameters this time are a 
					// bit more complex: (still "Ax^2 + Bx + C = 0")
					// a = edgeLength^2 * -velocityLength^2 + (edge . velocity)^2
					// b = edgeLength^2 * 2(velocity . spherePositionToVertex) - 2((edge . velocity)(edge . spherePositionToVertex))
					// c =  edgeLength^2 * (1 - spherePositionToVertexLength^2) + (edge . spherePositionToVertex)^2
					// . denotes dot product

					// edge v1v2
					testEdgeAndSphere(v1, v2, position, velocity, velocityLengthSquared, t, colliding_with_tri, collision);

					// Edge (v2, v3):
					testEdgeAndSphere(v2, v3, position, velocity, velocityLengthSquared, t, colliding_with_tri, collision);

					// Edge (v3, v1):
					testEdgeAndSphere(v3, v1, position, velocity, velocityLengthSquared, t, colliding_with_tri, collision);
				}

				if (!colliding_with_tri) {
					continue;
				}

				// the unit sphere (camera ellipsoid) collides with the face, therefore do the collision response

				// collision response
				float dist_to_collision = t * velocity.size();

				V3 new_base_point = position;
				V3 destination_point = position + velocity;

				// bring mesh closer to collision if necessary
				float very_close_distance = 0.25;
				
				if (dist_to_collision >= very_close_distance) {
					V3 V = velocity;
					V.normalize();
					V *= (dist_to_collision - very_close_distance);
					new_base_point = position + V * dt;

					V.normalize();
					collision -= V * very_close_distance * dt;
				}
				
				// camera sliding - my own algorithm
				
				// calculate sliding plane
				V3 slide_plane_normal = new_base_point - collision;
				slide_plane_normal.normalize();

				// move the camera down the velocity vector to the point of collision
				camera.physics.position = (new_base_point + velocity * t0) * ellipsoid;

				// calculate the vector that points along the triangle plane
				// by projecting the incoming direction of velocity onto the sliding plane normal
				// then using vector addition to calculate the vector along the plane.
				// everything here is normalised, therefore not in ellipsoid space.
				V3 along = normalised_velocity - (slide_plane_normal * vectorDotProduct(normalised_velocity, slide_plane_normal));

				// calculate the new velocity by applying the incoming speed (velocity size) to the direction
				camera.physics.velocity = along * camera.physics.velocity.size(); // TODO: should slow down as angle increases between incoming velocity and wall.
			}
		}

		// if there is physics data, use suvat and compute the new properties
		if (data != nullptr) {
			data->compute(dt); // TODO: Move above collision detection?
		}
	}

	// update the velocity for the camera (apply gravity)
	camera.physics.compute(dt);
};