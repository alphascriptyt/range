#pragma once

#include "uicomponent.h"
#include "textrenderer.h"

#include <vector>

// wrapper for all UI functionality
class UI {
	std::vector<UIComponent*> components;

public:
	bool init() {
		return true;
	}
	void update() {}
	void render() {}


};