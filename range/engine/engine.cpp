#pragma once

#include "engine.h"
#include "graphics/mesh.h"
#include "graphics/presets.h"
#include "utils.h"
#include "timer.h"

#include <iostream>
#include <fstream>

Engine::Engine() {
}

void Engine::onStartup() {
}

void Engine::onUpdate(float dt) {
}

bool Engine::setup(const std::string& window_name, int w, int h) {
	// create the renderer
	renderer = Renderer(w, h);
	renderer.camera = &camera;
	
	// try intialize the renderer
	if (!renderer.init(window_name)) {
		return false;
	}

	// try initialize the UI
	if (!ui.init()) {
		return false;
	}

	// force the mouse inside of the window
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// once everything has succeeded, trigger the onStartup event
	onStartup();

	// return success
	return true;
}

bool Engine::loadScene(std::string filename) {
	// start by clearing the current scene
	//Mesh::meshes = {};

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
			Mesh* mesh = new Mesh(mesh_path, size, colour);
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
			Mesh* light_mesh = new Mesh(Cube::vertices, Cube::faces, light_mesh_size, colour);
			light_mesh->makeLightSource(light);
		}
	}

	// succesfully loaded scene
	return true;
}

void Engine::handleEvents(float& dt) {
	dt *= 10; // TODO: sort this maybe? or just remove and scale up the rest magic value...

	SDL_Event ev;

	// handle current events
	while (SDL_PollEvent(&ev)) {
		// handle the current event
		switch (ev.type) {

		// exit gameloop when exit button is clicked
		case (SDL_QUIT):
			active = false;
			return;
		
		// handle keypresses
		case (SDL_KEYDOWN):
			switch (ev.key.keysym.sym) {
			case (SDLK_ESCAPE):
				// exit gameloop when escape is pressed
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
				camera.setMode(NOCLIP);
				break;

			case (SDLK_9):
				camera.setMode(FIRST_PERSON);
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
			renderer.camera->updateViewAngles(y, p);

			// calculate the camera's looking direction
			renderer.camera->direction.x = sin(renderer.camera->yaw);
			renderer.camera->direction.y = -sin(renderer.camera->pitch);
			renderer.camera->direction.z = cos(renderer.camera->yaw);
			renderer.camera->direction.normalize();

			break;
		}
	}

	// move camera
	if (handlingInput) {
		keyboardState = SDL_GetKeyboardState(NULL);
		renderer.camera->move(keyboardState, dt);
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

	Timer frame_timer;

	int fps = 0;
	float frame_time = 0;
	float dt = 0;

	Uint64 current_ticks = SDL_GetTicks();
	Uint64 last_ticks = current_ticks;

	FontRenderer title_font("C://Users//olive//source//repos//range//range//engine//res//font//louisgeorgecafe_base.ttf", COLOUR::GOLD, 28);

	KeyValueFontRenderer test_font_renderer("C://Users//olive//source//repos//range//range//engine//res//font//louisgeorgecafe_base.ttf", COLOUR::MAROON, 28);
	//KeyValueFontRenderer test_font_renderer("C://Windows//Fonts//segoeui.ttf", COLOUR::MAROON, 18);

	test_font_renderer.createKey("FPS: ");
	test_font_renderer.createKey("Frame Time: ");
	
	// mainloop 
	while (active) {
		// get initial ticks
		start_perf = SDL_GetPerformanceCounter();
		
		// handle the next event
		handleEvents(dt);
		
		// calculate delta time for time based movement		
		dt = getDeltaTime(current_ticks, last_ticks);

		// trigger the update event
		//onUpdate(dt);

		// perform physics
		physics.process(camera, dt);

		// render scene
		renderer.renderScene(Scene::scenes[0]);

		// trigger the update event
		onUpdate(dt);
		
		// every 200ms, display the fps
		if (frame_timer.elapsed() > 200) {
			// calculate performance
			fps = getCurrentFPS(start_perf); // calculate delta time and fps
			frame_time = frame_timer.elapsed() / 201.0f;

			// reset the timer
			frame_timer.reset();

			//Scene::scenes[0]->getLightSource("viewpoint")->colour = Colour((rand() % 255), (rand() % 255), (rand() % 255));
		}

		test_font_renderer.render(renderer.renderSurface, "FPS: ", std::to_string(fps), V2(50, 150));
		test_font_renderer.render(renderer.renderSurface, "Frame Time: ", std::to_string(frame_time), V2(50, 180));
		title_font.render(renderer.renderSurface, "Range", V2(450, 50));


		// render the buffer to the screen
		renderer.display();
	}
}

void Engine::exit() {
	// cleanup the renderer
	renderer.cleanup();

	// release SDL resources
	SDL_Quit();
}