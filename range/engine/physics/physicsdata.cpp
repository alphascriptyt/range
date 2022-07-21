#include "physicsdata.h"

void PhysicsData::updateVelocity(float dt) {
	// method to update the velocity

	// add to pre-existing force using F=MA
	force += acceleration * mass; 

	// calculate new velocity
	velocity += force / mass * dt; // TODO: Do we need to multiply by dt?
}

#include <iostream>
void PhysicsData::compute(float dt) {
	// calculate the new position
	position += velocity * dt;
	//std::cout << "Position: ";  position.print();

	// reset force
	force = V3();
}


