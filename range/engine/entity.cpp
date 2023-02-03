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
	M4 translation;
	translation.makeIdentity();
	
	translation[3][0] = physics->position.x;
	translation[3][1] = physics->position.y;
	translation[3][2] = physics->position.z;

	M4 rotation = makeRotationMatrix(mesh->yaw, mesh->pitch, mesh->roll);
	
	M4 scale;
	scale.makeIdentity();

	scale[0][0] = mesh->size.x;
	scale[1][1] = mesh->size.y;
	scale[2][2] = mesh->size.z;

	return scale * rotation * translation;
}	