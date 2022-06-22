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
	V3 pos_x(0, 0, 0);
	V3 size_x(10, 1, 1);
	Mesh mesh_x(Cube::vertices, Cube::faces, pos_x, size_x, COLOUR::WHITE);
	
	V3 pos_y(0, 1, 0);
	V3 size_y(1, 10, 1);
	Mesh mesh_y(Cube::vertices, Cube::faces, pos_y, size_y, COLOUR::WHITE);

	V3 pos_z(0, 0, 1);
	V3 size_z(1, 1, 10);
	Mesh mesh_z(Cube::vertices, Cube::faces, pos_z, size_z, COLOUR::WHITE);


	// start engine loop
	engine.loop();

	// cleanup
	engine.exit();
}