#include "physics.h"
#include "../scene.h"
#include "vectormaths.h"
#include <iostream>
#include "collision.h"
#include "AABB.h"

void Physics::process(float dt) {
	for (int i = 0; i < Scene::scenes[0]->objects.size(); ++i) {
		// get the scene object
		SceneObject* object = Scene::scenes[0]->objects[i];

		// get the physics data

		PhysicsData* data = object->physics;

		// find mesh bounding box, maybe save this for clipping??
		AABB aabb(object->mesh);

		// find collisions
		if (pointCollidesWithAABB(camera->position, aabb)) {
			object->mesh->fillColour(COLOUR::GREEN);
		}
		else {
			object->mesh->fillColour(COLOUR::RED);
		}

		// TODO: need to make the camera move based on velocity and acceleration rather than absolute position, then
		// we can handle collisions

		V3 acceleration(0, gravity, 0);

		// if there is physics data, use suvat
		if (data != nullptr) {
			// F = MA, calculate gravity, TODO: Precalculate?
			data->force = acceleration * data->mass;

			// calculate new velocity and position
			data->mass = 0.0000000001;
			data->velocity += data->force / data->mass * dt;
			//data->position += data->velocity * dt;
			//object->mesh->pos += data->velocity * dt;
		}
	}
};