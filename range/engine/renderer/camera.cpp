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

	// TODO: use enum for camera modes like mode = CAMERA_MODE_NOCLIP, but need to properly plan out tings.
	// TODO: Need to rethink all this but hopefully adding friction will solve.

	// handle based on camera mode
	if (mode == NOCLIP) {
		// moving forwards
		if (keys[SDL_SCANCODE_W]) {
			physics.position.x += dt * std::sin(yaw);
			physics.position.z += dt * std::cos(yaw);
		}

		// moving backwards
		if (keys[SDL_SCANCODE_S]) {
			physics.position.x -= dt * std::sin(yaw);
			physics.position.z -= dt * std::cos(yaw);
		}

		// moving right
		if (keys[SDL_SCANCODE_D]) {
			physics.position.x += dt * std::sin(yaw - HALF_PI);
			physics.position.z += dt * std::cos(yaw - HALF_PI);
		}

		// moving left
		if (keys[SDL_SCANCODE_A]) {
			physics.position.x += dt * std::sin(yaw + HALF_PI);
			physics.position.z += dt * std::cos(yaw + HALF_PI);
		}

		// moving down
		if (keys[SDL_SCANCODE_LSHIFT]) {
			physics.position.y -= dt;
		}

		// moving up
		if (keys[SDL_SCANCODE_SPACE]) {
			physics.position.y += dt;
		}
	}
	else {
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
			physics.velocity.x -= dt * std::sin(yaw);
			physics.velocity.z -= dt * std::cos(yaw);
			moved = true;
		}

		// moving right
		if (keys[SDL_SCANCODE_D]) {
			physics.velocity.x += dt * std::sin(yaw - HALF_PI);
			physics.velocity.z += dt * std::cos(yaw - HALF_PI);
			moved = true;
		}

		// moving left
		if (keys[SDL_SCANCODE_A]) {
			physics.velocity.x += dt * std::sin(yaw + HALF_PI);
			physics.velocity.z += dt * std::cos(yaw + HALF_PI);
			moved = true;
		}

		if (!moved) {
			physics.velocity.x = 0;
			physics.velocity.z = 0;
		}

		// moving down
		if (keys[SDL_SCANCODE_LSHIFT]) {
			physics.velocity.y -= dt;
		}

		// moving up
		if (keys[SDL_SCANCODE_SPACE]) {
			physics.velocity.y += dt;
		}
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

void Camera::setMode(int camera_mode) {
	// set the camera's mode
	mode = camera_mode;

	if (camera_mode == NOCLIP) {
		physics.useGravity = false;
		physics.velocity = V3();
	}
	else {
		physics.useGravity = true;
	}

}
