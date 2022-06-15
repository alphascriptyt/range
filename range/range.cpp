/*
plan: 
- get engine running first, optimise etc
- implement better moving/wall detection etc
- get basic multiplayer working
- make game


structure:
- how should the engine and game interact?

*/

#define SDL_MAIN_HANDLED // to allow for my own main function

#include "engine/engine.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/colour.h"
#include "engine/renderer/presets.h"

#include <iostream>

int main(int argc, char** argv) {
	Engine engine;

	// startup the window
	if (!engine.setup(960, 720)) {
		printf("Failed to startup.\n");
		return -1;
	}
	
	// keep engine loading of scenes so i can use 3denvcreator to make map?
	//engine.loadScene("C://Users//olive//OneDrive - York College//College Work//Computer Science//AS2//2nd Year Project//Coursework//Program//FINAL//Final Version (not uploaded)//3DEnvironmentCreator-OP//traffic.txt");
	V3 pos(0, 0, 10);
	V3 size(2, 2, 2);
	Mesh mesh(Cube::vertices, Cube::faces, pos, size, COLOUR::WHITE);
	
	// start engine loop
	engine.loop();

	// cleanup
	engine.exit();
}