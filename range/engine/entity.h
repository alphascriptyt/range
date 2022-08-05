#pragma once
#include "physics/physics.h"
#include "renderer/mesh.h"

// struct to store any physical entity in the scene
struct Entity {
	// properties
	Mesh* mesh = nullptr;
	PhysicsData* physics = nullptr;
	
	// mesh needs position, and so does physics


	// constructor
	Entity();
};