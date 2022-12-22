#pragma once

#include "vec3D.h"
#include "mat4D.h"
#include "../physics/physicsdata.h"

#include <SDL.h>
#include <cmath>

enum CameraModes {
	FIRST_PERSON,
	THIRD_PERSON,
	NOCLIP
};

class Camera {
	// internal methods
	void processPitch(float p); 	// normalize and set the camera's pitch
	void processYaw(float y); 		// normalize and set the camera's yaw

	int mode = FIRST_PERSON;


public:
	// properties
	PhysicsData physics;
	
	V3 direction = V3(0, 0, 0);	// a 3D vector of the cameras direction
	float pitch = 0;			// pitch viewangle
	float yaw = 0;				// yaw viewangle

	
	// constructors
	Camera();					// default constructor, position = {0,0,0}
	Camera(V3& pos);			// constructor to set the position of the camera

	// methods
	void move(const Uint8* keys, float dt);		// move camera position
	void updateViewAngles(float y, float p);	// update the camera's viewangles
	void setMode(int mode);
	M4 makeViewMatrix();
};