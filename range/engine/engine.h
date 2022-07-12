#pragma once

#include "sdl.h"
#include "renderer/renderer.h"
#include "physics/physics.h"
#include "renderer/camera.h"

#include <string>

class Engine {
	bool active = true;				// whether the engine should be running
	bool handlingInput = true;		// whether the engine should be proccessing inputs
	
	// performance methods
	int getCurrentFPS(Uint64& start_perf);							// get the current FPS
	float getDeltaTime(Uint64& current_ticks, Uint64& last_ticks);	// get the delta time (for stable physics)

	// internal methods
	void handleEvents(float& dt);	// handle all events 

	Camera camera;

public:
	// constructor
	Engine();
	
	// temp
	bool loadScene(std::string filename);

	// engine API
	Renderer renderer;				// the current renderer instance
	Physics physics;
	bool setup(int w, int h);		// setup the engine
	void loop();					// main engine loop
	void exit();					// cleanup all resources
};