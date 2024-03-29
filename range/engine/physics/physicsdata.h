#pragma once
#include "vec3d.h"

struct PhysicsData {
	// properties
	V3 position = V3(0, 0, 0);
	V3 velocity = V3(0, 0, 0);
	V3 force = V3(0, 0, 0);
	V3 acceleration = V3(0, -9.8, 0);

	float mass = 1; // TEMP

	// flags
	bool useGravity = true;

	// methods
	void updateVelocity(float dt);
	void compute(float dt);
};