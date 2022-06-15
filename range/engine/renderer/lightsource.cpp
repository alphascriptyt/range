#include "lightsource.h"
#include "colour.h"
#include "vec3D.h"

// initialize static member container of pointers to meshes
std::vector<LightSource*> LightSource::sources = {}; 

// constructor
LightSource::LightSource(V3& pos, Colour& c, float s) {
	position = pos;
	colour = c;
	strength = s;

	sources.push_back(this);
}