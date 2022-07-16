#pragma once
#include <vector>

#include "vec3d.h"
#include "../renderer/camera.h"

/*
Thinking about how objects in the game should work. Needs to be a hierarchy of objects.

Highest object maybe : SceneObject?

SceneObject has MeshData and PhysicsData

	- PhysicsData stores AABB information and properties
	- MeshData stores all vertices/faces/textures whatever

Stages of engine loop:
	- Engine handles SDL events
	- Physics takes the outcome of these events and updates all PhysicsData
	- Renderer then uses the updated SceneObject's positions, orientations, styles etc
		and renderes the MeshData.

Maybe have a Scene that stores lightsources and SceneObjects?
lightsources are not sceneobjects but their representing meshes may be


*/



class Physics {
	float gravity = -9.8;

public:
	void process(Camera& camera, float dt);
};

