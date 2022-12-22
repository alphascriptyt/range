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
void Camera::move(const Uint8* keys, float dt) {
	// move camera dependent on key press
	
	// flag to determine whether the camera was moved in a direction
	bool moved = false;

	// TODO: use enum for camera modes like mode = CAMERA_MODE_NOCLIP, but need to properly plan out tings.
	// TODO: Need to rethink all this but hopefully adding friction will solve.

	// handle based on camera mode
	if (mode == NOCLIP) {
		
		// moving forwards
		if (keys[SDL_SCANCODE_W]) {
			physics.position.x += dt * -sin(yaw);
			physics.position.z += dt * cos(yaw);
		}

		// moving backwards
		if (keys[SDL_SCANCODE_S]) {
			physics.position.x -= dt * -sin(yaw);
			physics.position.z -= dt * cos(yaw);
		}

		// moving right
		if (keys[SDL_SCANCODE_D]) {
			physics.position.x += dt * sin(yaw - HALF_PI);
			physics.position.z += dt * -cos(yaw - HALF_PI);
		}

		// moving left
		if (keys[SDL_SCANCODE_A]) {
			physics.position.x += dt * sin(yaw + HALF_PI);
			physics.position.z += dt * -cos(yaw + HALF_PI);
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
			physics.velocity.x += dt * sin(yaw);
			physics.velocity.z += dt * cos(yaw);

			//position.x += dt * std::sin(yaw);
			//position.z += dt * std::cos(yaw);
			moved = true;
		}

		// moving backwards
		if (keys[SDL_SCANCODE_S]) {
			physics.velocity.x -= dt * sin(yaw);
			physics.velocity.z -= dt * cos(yaw);
			moved = true;
		}

		// moving right
		if (keys[SDL_SCANCODE_D]) {
			physics.velocity.x += dt * sin(yaw - HALF_PI);
			physics.velocity.z += dt * cos(yaw - HALF_PI);
			moved = true;
		}

		// moving left
		if (keys[SDL_SCANCODE_A]) {
			physics.velocity.x += dt * sin(yaw + HALF_PI);
			physics.velocity.z += dt * cos(yaw + HALF_PI);
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

void Camera::processPitch(float p) {
	//pitch += p;
	pitch -= p;

	// so we don't look past 90 degrees behind the camera
	if (pitch > HALF_PI) {
		pitch = HALF_PI;
	}
	else if (pitch < -HALF_PI) {
		pitch = -HALF_PI;
	}
}

void Camera::processYaw(float y) {
	//yaw -= y;
	yaw += y;


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

M4 Camera::makeViewMatrix() {
	M4 translation;
	translation.makeIdentity();

	translation[0][3] = -physics.position.x;
	translation[1][3] = -physics.position.y;
	translation[2][3] = -physics.position.z;

	M4 rotation = makeRotationMatrix(-yaw, -pitch, 0);

	return rotation * translation;
}
