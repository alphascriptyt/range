#pragma once

#include "sdl.h"
#include "graphics/renderer.h"
#include "graphics/camera.h"
#include "physics/physics.h"
#include "ui/ui.h"


#include <string>

class Engine {
	bool active = true;				// whether the engine should be running
	bool handlingInput = true;		// whether the engine should be proccessing inputs
	
	// performance methods
	int getCurrentFPS(Uint64& start_perf);							// get the current FPS
	float getDeltaTime(Uint64& current_ticks, Uint64& last_ticks);	// get the delta time (for stable physics)

	// internal methods
	void handleEvents(float& dt);	// handle all events 

public:
	// constructor
	Engine();
	
	// temp
	bool loadScene(std::string filename);

	// main engine components
	Renderer renderer;				// the current renderer instance
	Camera camera;
	Physics physics;
	UI ui;

	// engine events to override for actual game
	virtual void onStartup();
	virtual void onUpdate(float dt);

	// properties
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

	// engine API
	bool setup(const std::string& window_name, int w, int h);	// setup the engine
	void loop();												// main engine loop
	void exit();												// cleanup all resources
};