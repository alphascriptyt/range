#include "mesh.h"

#include "utils.h"
#include "mathutils.h"

#include <cassert>
#include <string>
#include <fstream>
#include <iostream>

// constructors
Mesh::Mesh(std::vector<V3>& v, std::vector<std::vector<int>>& f, V3& p, Colour colour) {
	vertices = v;
	faces = f;
	pos = p;

	// set the mesh colour
	fillColour(colour);
}

Mesh::Mesh(std::vector<V3>& v, std::vector<std::vector<int>>& f, V3& p, V3& s, Colour colour) {
	vertices = v;
	faces = f;
	pos = p;
	size = s;

	// set the mesh colour
	fillColour(colour);
}

Mesh::Mesh(std::string filename, V3& p, Colour colour) {
	pos = p;
	path = filename;

	// try load the file
	if (loadFromObjFile(filename)) {
		// set the mesh colour
		fillColour(colour);
	}
}

Mesh::Mesh(std::string filename, V3& p, V3& s, Colour colour) {
	pos = p;
	size = s;
	path = filename;

	// try load the file
	if (loadFromObjFile(filename)) {
		// set the mesh colour
		fillColour(colour);
	}
}

// methods
bool Mesh::loadFromObjFile(std::string filename) {
	// open file
	std::ifstream file(filename);

	// test if file is open
	if (!file.is_open()) {
		std::cout << "Failed to load mesh from: " << filename << std::endl;
		return false;
	}

	std::string line;

	// read the file line by line
	while (std::getline(file, line)) {
		std::vector<std::string> parts = splitStringBy(line, ' ');

		// ensure the line is of correct format
		if (parts.size() < 4) { continue; }

		if (parts[0] == "f") {
			// if the line describes a face
			std::vector<int> face;
			for (int i = 1; i < parts.size(); ++i) {
				std::vector<std::string> verts = splitStringBy(parts[i], '/');

				// store each vertices that the face is made of 
				face.push_back(stoi(verts[0]) - 1);

			}
			faces.push_back(face);

		}
		else if (parts[0] == "v") {
			// if the line describes a vertex, store it
			vertices.push_back({ stof(parts[1]), stof(parts[2]), stof(parts[3]) });
		}
	}

	// succesfully imported the mesh from file
	return true;
}

void Mesh::fillColour(Colour colour) {
	// reset the face colours
	colours = {};

	// create a new colour from the colour to avoid bad references from the colour passed
	Colour* c = new Colour(colour.toInt()); // TODO: Why am I using pointers?

	// fill the mesh faces with the colour
	for (int i = 0; i < faces.size(); ++i) {
		colours.push_back(c);
	}
}

void Mesh::makeLightSource(LightSource* light) {
	absorbsLight = false;
	isLightSource = true;

	// set the mesh properties to the lights
	lightsource = light;
	fillColour(light->colour);
	pos = light->position;
}

void Mesh::move(const Uint8* keys, float dt, float camera_yaw) {
	// moving forwards
	if (keys[SDL_SCANCODE_W]) {
		pos.x += dt * std::sin(camera_yaw);
		pos.z += dt * std::cos(camera_yaw);
	}

	// moving backwards
	if (keys[SDL_SCANCODE_S]) {
		pos.x -= dt * std::sin(camera_yaw);
		pos.z -= dt * std::cos(camera_yaw);
	}

	// moving right
	if (keys[SDL_SCANCODE_D]) {
		pos.x -= dt * std::sin(camera_yaw + HALF_PI);
		pos.z -= dt * std::cos(camera_yaw + HALF_PI);

	}

	// moving left
	if (keys[SDL_SCANCODE_A]) {
		pos.x -= dt * std::sin(camera_yaw - HALF_PI);
		pos.z -= dt * std::cos(camera_yaw - HALF_PI);
	}

	// moving down
	if (keys[SDL_SCANCODE_LSHIFT]) {
		pos.y -= dt;
	}

	// moving up
	if (keys[SDL_SCANCODE_SPACE]) {
		pos.y += dt;
	}

	// if a mesh represents a lightsource, move the source too
	if (isLightSource && lightsource != nullptr) {
		lightsource->position = pos;
	}
}