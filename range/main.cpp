#define SDL_MAIN_HANDLED // to allow for my own main function

#include "engine/engine.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/colour.h"
#include "engine/renderer/presets.h"
#include "engine/physics/physics.h"



#include <iostream>

class Game : public Engine {
	void onStartup() {
		camera.setMode(NOCLIP);
	}

	void onUpdate(float dt) {
		if (keyboardState[SDL_SCANCODE_0]) {
			std::cout << "0" << std::endl;
		}
	}
}; 



/* TODO: List of things to do after holiday!

- Per vertex lighting (lerp colours per pixel)
- Ambient and specular lighting
- Optimise things like reusing AABB bounding box?
- Store center of mesh
- Sort out draw scan line
- Sort out line drawing glitches (random lines going too far and gap between triangles)
- Setup some debugging tools like drawing normals and collision boxes + better wireframe

*/

int main(int argc, char** argv) {
	//Engine engine;
	Game game;
	Scene scene;

	// setup the game and engine
	if (!game.setup("Range", 960, 720)) {
		printf("Failed to startup.\n");
		return -1;
	}
	
	// keep engine loading of scenes so i can use 3denvcreator to make map?
	//engine.loadScene("C://Users//olive//OneDrive - York College//College Work//Computer Science//AS2//2nd Year Project//Coursework//Program//FINAL//Final Version (not uploaded)//3DEnvironmentCreator-OP//traffic.txt");

	// create this map?
	// https://www.google.com/search?q=quake+map&tbm=isch&ved=2ahUKEwj3oKjr_uz4AhUJZ_EDHXREAagQ2-cCegQIABAA&oq=quake+map&gs_lcp=CgNpbWcQAzIFCAAQgAQyBQgAEIAEMgUIABCABDIFCAAQgAQyBggAEB4QBTIGCAAQHhAFMgYIABAeEAgyBggAEB4QCDIGCAAQHhAIMgYIABAeEAg6BAgAEBg6BAgjECc6CAgAEIAEELEDOgsIABCABBCxAxCDAToECAAQQzoECAAQAzoHCAAQsQMQQ1DjN1jTPWDpPmgAcAB4AIABvgGIAc8FkgEDOS4xmAEAoAEBqgELZ3dzLXdpei1pbWfAAQE&sclient=img&ei=UxPKYrerM4nOxc8P9IiFwAo&bih=658&biw=1523&rlz=1C1CHBF_en-GBGB914GB914#imgrc=WyNLVBQ3CtEPkM

	
	/*
	V3 mesh_pos(0, -3, 5);
	V3 mesh_size(20, 0.5, 20);
	Colour c = COLOUR::AQUA;

	Mesh mesh_floor(Cube::vertices, Cube::faces, mesh_size, c);
	
	PhysicsData physics;
	physics.position = mesh_pos;
	scene.createEntity("floor", mesh_floor, physics);
	*/
	
	V3 player_pos(0, 0, 3);
	V3 player_size(1, 1, 1);
	Mesh player_mesh(Cube::vertices, Cube::faces, player_size, COLOUR::AQUA);
	PhysicsData player_physics;
	player_physics.position = player_pos;
	scene.createEntity("player", player_mesh, player_physics);

	// start engine loop
	game.loop();

	// cleanup
	game.exit();
}