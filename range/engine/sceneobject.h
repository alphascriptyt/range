#pragma once
#include "physics/physics.h"
#include "renderer/mesh.h"

// struct to store any physical object in the scene
struct SceneObject {
	// properties
	Mesh* mesh = nullptr;
	PhysicsData* physics = nullptr;
	
	// constructor
	SceneObject();
};