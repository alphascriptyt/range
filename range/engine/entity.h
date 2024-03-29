#pragma once
#include "physics/physics.h"
#include "graphics/mesh.h"
#include "mat4D.h"

// struct to store any physical entity in the scene
struct Entity {
	// properties
	Mesh* mesh = nullptr;
	PhysicsData* physics = nullptr;
	
	// constructor
	Entity();

	// methods
	// events to override
	virtual void onStartup();
	virtual void onUpdate(float dt);

	void update(float dt);
	M4 makeModelMatrix();
};