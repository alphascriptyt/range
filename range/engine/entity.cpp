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