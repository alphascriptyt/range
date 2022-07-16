#include "physics.h"
#include "../scene.h"
#include "vectormaths.h"
#include <iostream>
#include "collision.h"
#include "AABB.h"
#include "physicsdata.h"
#include "../renderer/triangle3D.h"

// https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
static bool intersectRaySegmentSphere(V3 o, V3 d, V3 so, float radius2, V3& ip)
{
	//we pass in d non-normalized to keep it's length
	//then we use that length later to compare the intersection point to make sure
	//we're within the actual ray segment
	float l = d.size();
	d /= l;

	V3 m = o - so;
	float b = vectorDotProduct(m, d);
	float c = vectorDotProduct(m, m) - radius2;

	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
	if (c > 0.0f && b > 0.0f)
		return false;
	float discr = b * b - c;

	// A negative discriminant corresponds to ray missing sphere
	if (discr < 0.0f)
		return false;

	// Ray now found to intersect sphere, compute smallest t value of intersection
	float t = -b - sqrtf(discr);

	// If t is negative, ray started inside sphere so clamp t to zero
	if (t < 0.0f)
		t = 0.0f;
	ip = (d * t) + o;

	//here's that last segment check I was talking about
	if (t > l)
		return false;

	return true;
}

bool rayTriangleIntersect(V3& orig, V3& dir, V3& v0, V3& v1, V3& v2, float& t) {
	// compute plane's normal
	V3 v0v1 = v1 - v0;
	V3 v0v2 = v2 - v0;
	// no need to normalize
	V3 N = vectorCrossProduct(v0v1, v0v2);  //N 
	float area2 = N.size();

	// Step 1: finding P

	// check if ray and plane are parallel ?
	float NdotRayDirection = vectorDotProduct(N, dir);
	if (fabs(NdotRayDirection) < 0.00001)  //almost 0 kEpsilon?
		return false;  //they are parallel so they don't intersect ! 

	// compute d parameter using equation 2
	float d = -vectorDotProduct(v0, N);

	// compute t (equation 3)
	t = -(vectorDotProduct(orig, N) + d) / NdotRayDirection;

	// check if the triangle is in behind the ray
	if (t > 0) return false;   //the triangle is behind 
	
	// compute the intersection point using equation 1
	V3 P = orig + dir * t;

	// Step 2: inside-outside test
	V3 C;  //vector perpendicular to triangle's plane 
	
	// edge 0
	V3 edge0 = v1 - v0;
	V3 vp0 = P - v0;
	C = vectorCrossProduct(edge0, vp0);
	if (vectorDotProduct(N, C) < 0) return false;  //P is on the right side 
	
	// edge 1
	V3 edge1 = v2 - v1;
	V3 vp1 = P - v1;
	C = vectorCrossProduct(edge1, vp1);
	if (vectorDotProduct(N, C) < 0)  return false;  //P is on the right side 
	
	// edge 2
	V3 edge2 = v0 - v2;
	V3 vp2 = P - v2;
	C = vectorCrossProduct(edge2, vp2);
	if (vectorDotProduct(N, C) < 0) return false;  //P is on the right side; 
	
	return true;  //this ray hits the triangle 
}

bool intersectRaySphere(V3& origin, V3& direction, V3& center, float radius, float& t, V3& q) {
	V3 m = origin - center;
	float b = vectorDotProduct(m, direction);
	float c = vectorDotProduct(m, m) - (radius * radius);

	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
	if (c > 0.0f && b > 0.0f) return false;
	float discr = b * b - c;

	// A negative discriminant corresponds to ray missing sphere 
	if (discr < 0.0f) return false;

	// Ray now found to intersect sphere, compute smallest t value of intersection
	t = -b - sqrt(discr);

	// If t is negative, ray started inside sphere so clamp t to zero 
	if (t < 0.0f) t = 0.0f;
	q = origin + direction * t;
	
	return true;
}

float intersectSphere(V3& ray_origin, V3& ray_vector, V3& sO, double sR) {
	return 1;
}

void Physics::process(Camera& camera, float dt) {
	int count = 0;
	
	// update the velocity for the camera (apply gravity)
	camera.physics.updateVelocity(dt);
	
	// calculate intended camera destination
	V3 destination = camera.physics.position + camera.physics.velocity * dt; // * dt??

	for (int i = 0; i < Scene::scenes[0]->objects.size(); ++i) {
		// get the scene object
		SceneObject* object = Scene::scenes[0]->objects[i];

		// get the object's physics data
		PhysicsData* data = object->physics;

		// find mesh bounding box, maybe save this for clipping??
		AABB aabb(object->mesh);

		// find collisions with camera and mesh
		V3 intersection_old;
		float dist;
		if (pointCollidesWithAABB(destination, aabb, intersection_old, dist)) {
			count++;
			object->mesh->fillColour(COLOUR::GREEN);	
			
			// found a collision with the mesh, not sure if we should be doing it this way but oh well, may be a good optimisation
			// rather than checking every tri

			Mesh* mesh = object->mesh;
			float sphere_radius = 3;
			for (int i = 0; i < mesh->faces.size(); ++i) {
				// get mesh face vertices
				V3 v1(mesh->vertices[mesh->faces[i][0]].x, mesh->vertices[mesh->faces[i][0]].y, mesh->vertices[mesh->faces[i][0]].z);
				V3 v2(mesh->vertices[mesh->faces[i][1]].x, mesh->vertices[mesh->faces[i][1]].y, mesh->vertices[mesh->faces[i][1]].z);
				V3 v3(mesh->vertices[mesh->faces[i][2]].x, mesh->vertices[mesh->faces[i][2]].y, mesh->vertices[mesh->faces[i][2]].z);

				v1 *= mesh->size;
				v1 += data->position;

				v2 *= mesh->size;
				v2 += data->position;

				v3 *= mesh->size;
				v3 += data->position;

				// determine if the tri is a backface and ignore it if so
				V3 edge1 = v2 - v1;
				V3 edge2 = v3 - v1;
				V3 normal = vectorCrossProduct(edge1, edge2);
				normal.normalize();

				Plane tri_plane = Plane(normal, v1);

				V3 normalised_velocity = camera.physics.velocity;
				normalised_velocity.normalize();

				// determine if the triangle is a backface, velocity is direction
				float dot_product = vectorDotProduct(normalised_velocity, normal);

				// ignore backfaces as we cannot collide with them
				//if (dot_product >= 0) {
			//		std::cout << "dot failed" << std::endl;
			//		continue; 
			//	}

				// calculate signed distance from plane to camera position
				float d = findSignedDistance(destination, tri_plane); // should this be destination??

				// sphere does not intersect plane
				if (fabs(d) > sphere_radius) {
					std::cout << "fabs failed" << std::endl;
					continue;
				}
				float radius_squared = sphere_radius * sphere_radius;

				// the ray vector is the camera.physics.velocity
				V3 ray_vector = normalised_velocity;
				V3 ray_origin = camera.physics.position;
				//std::cout << "Dir: "; ray_vector.print();
				V3 intersection;
				//std::cout << "D: " << d << std::endl;


				bool outside_verts = false;
				if ((v1 - destination).sizeSquared() > radius_squared && 
					(v2 - destination).sizeSquared() > radius_squared && 
					(v3 - destination).sizeSquared() > radius_squared) {
					std::cout << "outside verts" << std::endl;
					outside_verts = true;
				}

				bool outside_edges = false;

				V3 a = v2 - v1;
				V3 b = v3 - v2;
				V3 c = v1 - v3;

				V3 ip;
				if (!intersectRaySegmentSphere(v1, a, destination, radius_squared, ip) &&
					!intersectRaySegmentSphere(v2, b, destination, radius_squared, ip) &&
					!intersectRaySegmentSphere(v3, c, destination, radius_squared, ip)) {
					//sphere outside of all triangle edges
					outside_edges = true;
					std::cout << "outside edges" << std::endl;
				}

				float t;
				if (!rayTriangleIntersect(ray_origin, ray_vector, v1, v2, v3, t) && outside_verts && outside_edges) {
					std::cout << "all failed" << std::endl;
					continue;
				}

				intersection = ray_origin + (ray_vector * t * dt); // CORRECT INTERSECTION.
				


				camera.physics.velocity = intersection - camera.physics.position;
				camera.physics.velocity.normalize();

				// ISSUE: when we move and then normalize, because we have extra x/z movement, there is less y, meaning the 
				// camera dips below, so what do we need to scale (intersection - camera.physics.position) by?


				//camera.physics.velocity *= d;
				//camera.physics.position = intersection;
				//camera.physics.velocity = V3();
				//camera.physics.acceleration = V3();
				
				// respond to collision
				mesh->colours[i] = &COLOUR::GOLD;
				printf("response\n");
				count++;

			}
		}
		else {
			object->mesh->fillColour(COLOUR::RED);
		}
		
		// if there is physics data, use suvat and compute the new properties
		if (data != nullptr) {
			data->compute(dt); // TODO: Move above collision detection?
		}
	}

	camera.physics.compute(dt);

	
};