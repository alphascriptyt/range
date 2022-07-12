#pragma once
#include "renderer/lightsource.h"
#include "sceneobject.h"

// class to represent the current scene
class Scene {
public:
	// keep track of all the scenes
	static std::vector<Scene*> scenes;
	
	// constructor
	Scene();

	// scene contents
	std::vector<SceneObject*> objects;			// the scene objects
	std::vector<std::string> objectKeys;		// the keys for each scene object

	std::vector<LightSource*> lightSources;		// the light sources in the scene
	std::vector<std::string> lightSourceKeys;	// the keys for each light source

	// methods for adding to scene
	LightSource* createLightSource(std::string& key, V3& pos, Colour& colour, float strength);
	SceneObject* createSceneObject(std::string& key, Mesh& mesh, PhysicsData& physics);

	// methods for getting scene contents
	LightSource* getLightSource(std::string& key);
	SceneObject* getSceneObject(std::string& key);
};