#pragma once

#include <iostream>
#include <fstream>

#include "engine.h"
#include "renderer/mesh.h"
#include "utils.h"
#include "renderer/presets.h"
#include "timer.h"

Engine::Engine() {
}

bool Engine::setup(int w, int h) {
	// create the renderer
	renderer = Renderer(w, h);

	// try intialize the renderer
	if (!renderer.init()) {
		return false;
	}

	// force the mouse inside of the window
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// return success
	return true;
}

bool Engine::loadScene(std::string filename) {
	// start by clearing the current scene
	Mesh::meshes = {};

	// clear all lightsources except the viewpoint
	LightSource::sources.erase(LightSource::sources.begin() + 1, LightSource::sources.end());

	// open the file
	std::ifstream file(filename);
	std::string line;

	// read the file line by line
	while (std::getline(file, line)) {
		std::vector<std::string> parts = splitStringBy(line, '|'); // | is used to split components (because a dir could contain spaces)

		// determine what the line describes
		if (parts[0] == "mesh") {
			// get the path of the mesh
			std::string mesh_path = parts[1];

			// position is first 3 numbers
			V3 pos(std::stof(parts[2]), std::stof(parts[3]), std::stof(parts[4]));

			// size is second set of 3 numbers
			V3 size(std::stof(parts[5]), std::stof(parts[6]), std::stof(parts[7]));

			// colour is the number after pos and size
			Colour colour(static_cast<uint32_t>(std::stoul(parts[8])));

			// create a mesh
			Mesh* mesh = new Mesh(mesh_path, pos, size, colour);
		}
		else if (parts[0] == "light") {
			// position is first 3 numbers
			V3 pos(std::stof(parts[1]), std::stof(parts[2]), std::stof(parts[3]));

			// get the strength
			float strength = std::stof(parts[4]);

			// colour is the number after pos and strength
			Colour colour(static_cast<uint32_t>(std::stoul(parts[5])));

			// create light source
			LightSource* light = new LightSource(pos, colour, strength);

			V3 light_mesh_size(0.2, 0.2, 0.2);
			Mesh* light_mesh = new Mesh(Cube::vertices, Cube::faces, pos, light_mesh_size, colour);
			light_mesh->makeLightSource(light);
		}
	}

	// succesfully loaded scene
	return true;
}

void Engine::handleEvents(float& dt) {
	dt *= 10; // magic value...

	SDL_Event ev;

	// handle current events
	while (SDL_PollEvent(&ev)) {
		// handle the current event type
		switch (ev.type) {

		// exit gameloop on quit
		case (SDL_QUIT):
			active = false;
			return;
		
			// handle keypresses
		case (SDL_KEYDOWN):
			// close window on escape
			switch (ev.key.keysym.sym) {
			case (SDLK_ESCAPE):
				active = false;
				break;

			case (SDLK_TAB):
				if (SDL_GetRelativeMouseMode()) {
					SDL_SetRelativeMouseMode(SDL_FALSE);
					handlingInput = false;
				}
				else {
					SDL_SetRelativeMouseMode(SDL_TRUE);
					handlingInput = true;
				}
				break;

			case (SDLK_0):
				renderer.camera.position.print();
				break;
			}

			break;

			// handle mouse movement
		case (SDL_MOUSEMOTION):
			// only process mousemotion if camera if handlingInput
			if (!handlingInput) { break; }

			// calculate change in pitch and yaw
			float sensitivity = 0.1; // hardcoded mouse sensitivity 

			// calculate pitch and yaw values
			float p = toRadians((float)ev.motion.yrel * sensitivity);
			float y = toRadians((float)ev.motion.xrel * sensitivity);

			// normalize and set the changes in viewangles
			renderer.camera.updateViewAngles(y, p);

			// calculate the camera's looking direction
			renderer.camera.direction.x = std::sin(renderer.camera.yaw);
			renderer.camera.direction.y = -std::sin(renderer.camera.pitch);
			renderer.camera.direction.z = std::cos(renderer.camera.yaw);
			renderer.camera.direction.normalize();

			break;
		}
	}

	// move camera
	if (handlingInput) {
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		renderer.camera.move(keys, dt);
	}
}

int Engine::getCurrentFPS(Uint64& start_perf) {
	// calculate fps ~ 1 / (delta_ticks / tick_rate)
	return 1.0f / ((SDL_GetPerformanceCounter() - start_perf) / (float)SDL_GetPerformanceFrequency());
}

float Engine::getDeltaTime(Uint64& current_ticks, Uint64& last_ticks) {
	// calculate delta time
	current_ticks = SDL_GetTicks();
	float dt = (current_ticks - last_ticks) / 1000.0f;
	last_ticks = current_ticks;

	return dt;
}

void Engine::loop() {
	// initialize timing/profiling values
	Uint64 start_perf = 0;

	int fps = 0;
	float dt = 0;

	Uint64 current_ticks = SDL_GetTicks();
	Uint64 last_ticks = current_ticks;

	// mainloop 
	while (active) {
		// get initial ticks
		start_perf = SDL_GetPerformanceCounter();
		
		// handle the next event
		handleEvents(dt);

		// calculate delta time for time based movement		
		dt = getDeltaTime(current_ticks, last_ticks);

		// render scene
		renderer.render();

		// calculate performance
		fps = getCurrentFPS(start_perf); // calculate delta time and fps

		// TEMP: display the FPS
		SDL_SetWindowTitle(renderer.window, std::to_string(fps).c_str());
	}
}

void Engine::exit() {
	// release SDL resources
	SDL_Quit();
}