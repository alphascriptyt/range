#include "camera.h"
#include "mathutils.h"
#include "vectormaths.h"

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

	V3 forward(direction.x, 0, direction.z);
	forward.normalize();

	if (mode == NOCLIP) {
		// moving forwards
		if (keys[SDL_SCANCODE_W]) {
			physics.position += forward * dt;
		}
		if (keys[SDL_SCANCODE_S]){
			physics.position -= forward * dt;
		}
		if (keys[SDL_SCANCODE_A]) {
			V3 up = V3(0, 1, 0);
			V3 right = vectorCrossProduct(up, forward);
			right.normalize();
			physics.position -= right * dt;
		}
		if (keys[SDL_SCANCODE_D]) {
			V3 up = V3(0, 1, 0);
			V3 right = vectorCrossProduct(up, forward);
			right.normalize();
			physics.position += right * dt;
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

	/*
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
			physics.position.x += dt * sin(yaw + HALF_PI);
			physics.position.z += dt * -cos(yaw + HALF_PI);
			//physics.position.x += dt * sin(yaw + HALF_PI);
			//physics.position.z += dt * -cos(yaw + HALF_PI);
		}

		// moving left
		if (keys[SDL_SCANCODE_A]) {
			physics.position.x += dt * sin(yaw - HALF_PI);
			physics.position.z += dt * -cos(yaw - HALF_PI);
			//physics.position.x += dt * sin(yaw - HALF_PI);
			//physics.position.z += dt * -cos(yaw - HALF_PI);
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
	*/
}

void Camera::processPitch(float p) {
	//pitch += p;
	pitch -= p;

	float limit = HALF_PI - 0.001;

	// so we don't look past 90 degrees behind the camera
	if (pitch > limit) {
		pitch = limit;
	}
	else if (pitch < -limit) {
		pitch = -limit;
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
	// TODO: I feel like this is the issue.
	// Definitely something wrong.

	// 1. Position = known
	// 2. Calculate cameraDirection
	//V3 zaxis = physics.position - direction;
	//V3 zaxis = direction - physics.position;
	//zaxis.normalize();
	/*
	V3 zaxis = direction;

	// 3. Get positive right axis vector
	V3 worldUp(0, 1, 0);
	V3 xaxis = vectorCrossProduct(worldUp, zaxis);
	xaxis.normalize();

	// 4. Calculate camera up vector
	V3 yaxis = vectorCrossProduct(zaxis, xaxis);
	yaxis.normalize();

	// create translation and rotation matrix
	M4 translation;
	translation.makeIdentity();
	translation[3][0] = -physics.position.x;
	translation[3][1] = -physics.position.y;
	translation[3][2] = -physics.position.z;

	M4 rotation;
	rotation.makeIdentity();
	rotation[0][0] = xaxis.x;
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x;
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x;
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;



	
	/*
	// inverse of camera's model matrix
	M4 translation;
	translation.makeIdentity();
	translation[3][0] = -physics.position.x;
	translation[3][1] = -physics.position.y;
	translation[3][2] = -physics.position.z;
	
	M4 rotation = makeRotationMatrix(-yaw, -pitch, -roll);
	//rotation.makeIdentity();

	// combine the transformations, order is reversed
	// because the transformation matrices are inverted
	//return rotation * translation; 
	
	*/
	//return translation * rotation;
	

	return makeLookAtMatrix(physics.position * -1, direction);
}
