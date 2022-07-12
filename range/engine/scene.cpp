#include "scene.h"
#include "renderer/mesh.h"

// initialize static member container of pointers to scenes
std::vector<Scene*> Scene::scenes = {};

// constructor
Scene::Scene() {
	// add the new scene to the scenes container
	scenes.push_back(this);
}

// methods
LightSource* Scene::createLightSource(std::string& key, V3& pos, Colour& colour, float strength) {
	// create the new light source
	LightSource* light = new LightSource(pos, colour, strength);

	// add the light source to the container
	lightSources.push_back(light);

	// add the light source key to the container
	lightSourceKeys.push_back(key);

	return light;
}

SceneObject* Scene::createSceneObject(std::string& key, Mesh& mesh, PhysicsData& physics) {
	// create the new scene object
	SceneObject* object = new SceneObject();

	// initialize object data
	object->mesh = &mesh;
	object->physics = &physics;

	// add the scene object to the container
	objects.push_back(object);

	// add the scene object key to the container
	objectKeys.push_back(key);

	return object;
}

LightSource* Scene::getLightSource(std::string& key) {
	// search for the matching key
	for (int i = 0; i < lightSourceKeys.size(); ++i) {
		if (lightSourceKeys[i] == key) {
			return lightSources[i];
		}
	}

	// failed to find matching key
	return nullptr;
}

SceneObject* Scene::getSceneObject(std::string& key) {
	// search for the matching key
	for (int i = 0; i < objectKeys.size(); ++i) {
		if (objectKeys[i] == key) {
			return objects[i];
		}
	}

	// failed to find matching key
	return nullptr;
}