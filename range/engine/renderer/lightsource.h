#pragma once

#include "vec3D.h"
#include "colour.h"
#include <vector>

class LightSource {
public:
	// globally keep track of all lightsources in the scene
	static std::vector<LightSource*> sources; 

	bool enabled = true; // whether the lightsource is 'turned on'

	V3 position = V3(0, 0, 0);				
	float strength = 1.0f;					
	Colour colour = Colour(255, 255, 255);	

	// constructor accepting a position, colour and strength
	LightSource(V3& pos, Colour& c, float s);
};
