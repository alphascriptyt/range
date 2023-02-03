#pragma once

#include <vector>
#include <string>

#include "vec3D.h"
#include "colour.h"
#include "lightsource.h"
#include "mat4D.h"

class Mesh {
public:										
	void calculateCenter();

	// mesh shape properties
	std::vector<V3> vertices = {};
	std::vector<std::vector<int>> faces = {};
	std::vector<Colour> colours = {};

	// mesh properties
	V3 size = V3(1, 1, 1);
	V3 center = V3();
	float pitch = 0;
	float yaw = 0;
	float roll = 0;
	bool absorbsLight = true;				// can be toggled off to simple render its normal colour without lighting
	bool isLightSource = false;				// used for representing light source
	LightSource* lightsource = nullptr;		// stores the lightsource if the mesh represents one
	std::string path = "";					// stores the path if the mesh was made from one

	// constructors
	Mesh(std::vector<V3>& v, std::vector<std::vector<int>>& f, const Colour& colour = COLOUR::WHITE);
	Mesh(std::vector<V3>& v, std::vector<std::vector<int>>& f, const V3& s, const Colour& colour = COLOUR::WHITE);
	Mesh(const std::string& filename, const Colour& colour = COLOUR::WHITE);
	Mesh(const std::string& filename, const V3& s, const Colour& colour = COLOUR::WHITE);
	
	// methods
	bool loadFromObjFile(const std::string& filename);
	void fillColour(const Colour& colour);
	void makeLightSource(LightSource* light);
	void move(const Uint8* keys, float dt, float camera_yaw);
};
