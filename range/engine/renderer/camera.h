#pragma once

#include "vec3D.h"

#include <SDL.h>
#include <cmath>

class Camera {
public:
	V3 position = V3(0, 0, 0);	// a 3D vector of the cameras position
	V3 direction = V3(0, 0, 0);	// a 3D vector of the cameras direction
	float pitch = 0;
	float yaw = 0;

	Camera();					// default constructor, position = {0,0,0}
	Camera(V3& pos);			// constructor to set the position of the camera

	void move(const Uint8* keys, float dt); // move camera position
	void processPitch(float p); 	// normalize and set the camera's pitch
	void processYaw(float p); 		// normalize and set the camera's yaw
};