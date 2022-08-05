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
LightSource* Scene::createLightSource(const std::string& key, V3& pos, Colour& colour, float strength) {
	// create the new light source
	LightSource* light = new LightSource(pos, colour, strength);

	// add the light source to the container
	lightSources.push_back(light);

	// add the light source key to the container
	lightSourceKeys.push_back(key);

	return light;
}

Entity* Scene::createEntity(const std::string& key, Mesh& mesh, PhysicsData& physics) {
	// create the new scene entity
	Entity* entity = new Entity();

	// initialize entity data
	entity->mesh = &mesh;
	entity->physics = &physics;

	// add the scene entity to the container
	entities.push_back(entity);

	// add the scene entity key to the container
	entityKeys.push_back(key);

	return entity;
}

LightSource* Scene::getLightSource(const std::string& key) {
	// search for the matching key
	for (int i = 0; i < lightSourceKeys.size(); ++i) {
		if (lightSourceKeys[i] == key) {
			return lightSources[i];
		}
	}

	// failed to find matching key
	return nullptr;
}

Entity* Scene::getEntity(const std::string& key) {
	// search for the matching key
	for (int i = 0; i < entityKeys.size(); ++i) {
		if (entityKeys[i] == key) {
			return entities[i];
		}
	}

	// failed to find matching key
	return nullptr;
}