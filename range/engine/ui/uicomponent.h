#pragma once

#include "vec2D.h"

// virtual base class to be overriden for custom UI elements
class UIComponent {
	bool enabled = true;

protected:
	V2 position;

public:
	UIComponent(const V2& pos) {
		position = pos;
	}

	virtual void update() {}
	virtual void render() {}
};