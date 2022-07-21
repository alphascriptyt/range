#pragma once
#include "physics/physics.h"
#include "renderer/mesh.h"

// struct to store any physical entity in the scene
struct Entity {
	// properties
	Mesh* mesh = nullptr;
	PhysicsData* physics = nullptr;
	
	// TODO: Entity?

	// constructor
	Entity();
};