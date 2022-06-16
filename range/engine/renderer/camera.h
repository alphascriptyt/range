#pragma once

#include "vec3D.h"

#include <SDL.h>
#include <cmath>

class Camera {
	// internal methods
	void processPitch(float p); 	// normalize and set the camera's pitch
	void processYaw(float y); 		// normalize and set the camera's yaw

public:
	// properties
	V3 position = V3(0, 0, 0);	// a 3D vector of the cameras position
	V3 direction = V3(0, 0, 0);	// a 3D vector of the cameras direction
	float pitch = 0;			// pitch viewangle
	float yaw = 0;				// yaw viewangle

	// constructors
	Camera();					// default constructor, position = {0,0,0}
	Camera(V3& pos);			// constructor to set the position of the camera

	// methods
	void move(const Uint8* keys, float dt);		// move camera position
	void updateViewAngles(float y, float p);	// update the camera's viewangles
	
};