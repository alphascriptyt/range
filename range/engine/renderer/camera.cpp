#include "camera.h"
#include "mathutils.h"

#include <iostream>

// constructors
Camera::Camera() {
	physics.position = V3(); // set to default
}

Camera::Camera(V3& pos) {
	physics.position = pos;	 // set to custom position
}

// methods

// move camera dependent on key press
void Camera::move(const Uint8* keys, float dt) {
	// flag to determine whether the camera was moved in a direction
	bool moved = false;

	// moving forwards
	if (keys[SDL_SCANCODE_W]) {
		physics.velocity.x += dt * std::sin(yaw);
		physics.velocity.z += dt * std::cos(yaw);
		//position.x += dt * std::sin(yaw);
		//position.z += dt * std::cos(yaw);
		moved = true;
	}

	// moving backwards
	if (keys[SDL_SCANCODE_S]) {
		//position.x -= dt * std::sin(yaw);
		//position.z -= dt * std::cos(yaw);
		physics.velocity.x -= dt * std::sin(yaw);
		physics.velocity.z -= dt * std::cos(yaw);
		moved = true;
	}

	// moving right
	if (keys[SDL_SCANCODE_D]) {
		//position.x += dt * std::sin(yaw - HALF_PI);
		//position.z += dt * std::cos(yaw - HALF_PI);
		physics.velocity.x += dt * std::sin(yaw - HALF_PI);
		physics.velocity.z += dt * std::cos(yaw - HALF_PI);
		moved = true;
	}

	// moving left
	if (keys[SDL_SCANCODE_A]) {
		//position.x += dt * std::sin(yaw + HALF_PI);
		//position.z += dt * std::cos(yaw + HALF_PI);
		physics.velocity.x += dt * std::sin(yaw + HALF_PI);
		physics.velocity.z += dt * std::cos(yaw + HALF_PI);
		moved = true;
	}

	if (!moved) {
		physics.velocity.x = 0;
		physics.velocity.z = 0;
	}

	moved = false;

	// moving down
	if (keys[SDL_SCANCODE_LSHIFT]) {
		//physics.position.y -= dt;
		physics.velocity.y -= dt;
		moved = true;
	}

	// moving up
	if (keys[SDL_SCANCODE_SPACE]) {
		//physics.position.y += dt;
		physics.velocity.y += dt;
		moved = true;
	}

	if (!moved) {
		physics.velocity.y = 0;
	}
}

// viewangle normalization (so the camera doesn't 'break its neck')
void Camera::processPitch(float p) {
	pitch += p;

	// so we don't look past 90 degrees behind the camera
	if (pitch > HALF_PI) {
		pitch = HALF_PI;
	}
	else if (pitch < -HALF_PI) {
		pitch = -HALF_PI;
	}
}

void Camera::processYaw(float y) {
	yaw -= y;

	// reset yaw after full 360 spin
	if (yaw < -TWO_PI) {
		// need to account for the amount past
		yaw -= -TWO_PI;

	}
	else if (yaw > TWO_PI) {
		// need to account for the amount past
		yaw -= TWO_PI;
	}
}

void Camera::updateViewAngles(float y, float p) {
	processYaw(y);
	processPitch(p);
}