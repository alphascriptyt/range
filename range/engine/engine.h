#pragma once

#include "sdl.h"
#include "renderer/renderer.h"

#include <string>

class Engine {
	bool active = true;				// whether the engine should be processing anything
	bool handlingInput = true;		// whether the engine should be proccessing inputs
	
public:
	Renderer renderer;				// the current renderer instance

	// temp
	bool loadScene(std::string filename);

	// constructor
	Engine();

	// performance methods
	int getCurrentFPS(Uint64& start_perf);							// get the current FPS
	float getDeltaTime(Uint64& current_ticks, Uint64& last_ticks);	// get the delta time (for stable physics)

	// engine API
	bool setup(int w, int h);		// setup the engine
	void handleEvents(float& dt);	// handle all events 
	void loop();					// main engine loop
	void exit();					// cleanup all resources
};