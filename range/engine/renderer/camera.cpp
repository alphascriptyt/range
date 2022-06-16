#include "camera.h"
#include "mathutils.h"

#include <iostream>


// constructors
Camera::Camera() {
	position = V3(); // set to default
}

Camera::Camera(V3& pos) {
	position = pos;	 // set to custom position
}

// methods

// move camera dependent on key press
void Camera::move(const Uint8* keys, float dt) {
	// moving forwards
	if (keys[SDL_SCANCODE_W]) {
		position.x += dt * std::sin(yaw);
		position.z += dt * std::cos(yaw);
	}

	// moving backwards
	if (keys[SDL_SCANCODE_S]) {
		position.x -= dt * std::sin(yaw);
		position.z -= dt * std::cos(yaw);
	}

	// moving right
	if (keys[SDL_SCANCODE_D]) {
		position.x += dt * std::sin(yaw - HALF_PI);
		position.z += dt * std::cos(yaw - HALF_PI);
	}

	// moving left
	if (keys[SDL_SCANCODE_A]) {
		position.x += dt * std::sin(yaw + HALF_PI);
		position.z += dt * std::cos(yaw + HALF_PI);
	}

	// moving down
	if (keys[SDL_SCANCODE_LSHIFT]) {
		position.y -= dt;
	}

	// moving up
	if (keys[SDL_SCANCODE_SPACE]) {
		position.y += dt;
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