#include "entity.h"

// constructor
Entity::Entity() {

}

// events to override
void Entity::onStartup() {
}

void Entity::onUpdate(float dt) {

}

void Entity::update(float dt) {
	onUpdate(dt);
	physics->compute(dt);
}

M4 Entity::makeModelMatrix() {
	// TODO: Only call this when we change a property?
	M4 m;
	m.makeIdentity();

	m[0][0] = mesh->size.x;
	m[1][1] = mesh->size.y;
	m[2][2] = mesh->size.z;

	m[0][3] = physics->position.x;
	m[1][3] = physics->position.y;
	m[2][3] = physics->position.z;

	// TODO: Add rotation

	return m;

}