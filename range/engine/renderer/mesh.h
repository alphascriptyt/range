#pragma once

#include <vector>
#include <string>

#include "vec3D.h"
#include "colour.h"
#include "lightsource.h"

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
	bool absorbsLight = true; // can be toggled off to simple render its normal colour without lighting
	bool isLightSource = false; // used for representing light source
	LightSource* lightsource = nullptr; // stores the lightsource if the mesh represents one
	std::string path = ""; // stores the path if the mesh was made from one

	// constructor taking a reference to the vertices, faces and position of the mesh
	Mesh(std::vector<V3>& v, std::vector<std::vector<int>>& f, Colour colour = COLOUR::WHITE);

	// constructor taking a reference to the vertices, faces, position and size of the mesh
	Mesh(std::vector<V3>& v, std::vector<std::vector<int>>& f, V3& s, Colour colour = COLOUR::WHITE);

	// constructor taking a filename and a reference to the position of the mesh
	Mesh(std::string filename, Colour colour = COLOUR::WHITE);

	// constructor taking a filename and references to the position and size of the mesh
	Mesh(std::string filename, V3& s, Colour colour = COLOUR::WHITE);

	// method to load mesh from a file
	bool loadFromObjFile(std::string filename);

	// method to fill the mesh with colour
	void fillColour(Colour colour);

	// method to make the mesh represent a given lightsource
	void makeLightSource(LightSource* light);

	// method to translate mesh
	void move(const Uint8* keys, float dt, float camera_yaw);
};
