#pragma once
#include "renderer/lightsource.h"
#include "entity.h"

// class to represent the current scene
class Scene {
public:
	// keep track of all the scenes
	static std::vector<Scene*> scenes;
	
	// constructor
	Scene();

	// scene contents
	std::vector<Entity*> entities;				// the scene objects
	std::vector<std::string> entityKeys;		// the keys for each scene object

	std::vector<LightSource*> lightSources;		// the light sources in the scene
	std::vector<std::string> lightSourceKeys;	// the keys for each light source

	// methods for adding to scene
	LightSource* createLightSource(std::string& key, V3& pos, Colour& colour, float strength);
	Entity* createEntity(std::string& key, Mesh& mesh, PhysicsData& physics);

	// methods for getting scene contents
	LightSource* getLightSource(std::string& key);
	Entity* getEntity(std::string& key);
};