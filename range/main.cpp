#define SDL_MAIN_HANDLED // to allow for my own main function

#include "engine/engine.h"
#include "engine/graphics/mesh.h"
#include "engine/graphics/colour.h"
#include "engine/graphics/presets.h"
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



/* 
TODO:
- Ambient and specular lighting
- Optimise things like reusing AABB bounding box?
- Store center of mesh
- Fully fix line rendering glithces
- Sort out line drawing glitches (random lines going too far and gap between triangles)
- Setup some debugging tools like drawing normals and collision boxes + better wireframe

*/

/*
Engine Restructure:

Aims:
- Make engine easier to work on
- Don't overdo the OOP, move unnecessary parts out of classes (eg drawTriangle)
- Make classes smaller and less forced, don't force it.

*/

int main(int argc, char** argv) {
	//Engine engine;
	Game game;
	Scene scene;

	// setup the game and engine
	if (!game.setup("Range", 1280, 720)) {
		printf("Failed to startup.\n");
		return -1;
	}
	
	// keep engine loading of scenes so i can use 3denvcreator to make map?


	// create this map?
	// https://www.google.com/search?q=quake+map&tbm=isch&ved=2ahUKEwj3oKjr_uz4AhUJZ_EDHXREAagQ2-cCegQIABAA&oq=quake+map&gs_lcp=CgNpbWcQAzIFCAAQgAQyBQgAEIAEMgUIABCABDIFCAAQgAQyBggAEB4QBTIGCAAQHhAFMgYIABAeEAgyBggAEB4QCDIGCAAQHhAIMgYIABAeEAg6BAgAEBg6BAgjECc6CAgAEIAEELEDOgsIABCABBCxAxCDAToECAAQQzoECAAQAzoHCAAQsQMQQ1DjN1jTPWDpPmgAcAB4AIABvgGIAc8FkgEDOS4xmAEAoAEBqgELZ3dzLXdpei1pbWfAAQE&sclient=img&ei=UxPKYrerM4nOxc8P9IiFwAo&bih=658&biw=1523&rlz=1C1CHBF_en-GBGB914GB914#imgrc=WyNLVBQ3CtEPkM

	// https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/DrawTriangle/
	// copy this sort of style for the draw triangle function?
	// i want to take it out of renderer as I don't like it and move it to a 2d primitive file or something?
	// or move it into triangl2d?


	
	V3 mesh_pos(0, 5, 0);

	//Mesh mesh_floor(Cube::vertices, Cube::faces, mesh_size, c);
	Mesh room("C://Users//olive//Desktop//room.obj", V3(3, 1, 3), COLOUR::WHITE);

	PhysicsData physics;
	physics.position = mesh_pos;
	scene.createEntity("room", room, physics);

	// TODO: Get lighting working as expected.
	//			Something to do with the camera direction.

	LightSource light(V3(0, 5, 0), COLOUR::RED, 1);

	Mesh source(Cube::vertices, Cube::faces, V3(1, 1, 1), COLOUR::RED);
	PhysicsData cube_physics;
	cube_physics.position = V3(0, 10, 0);
	scene.createEntity("source_cube", source, cube_physics);
	
	/*
	V3 mesh_pos(0, -3, 5);
	V3 mesh_size(2, 2, 20);
	Colour c = COLOUR::MAROON;

	Mesh cube(Cube::vertices, Cube::faces, mesh_size, c);

	PhysicsData physics;
	physics.position = mesh_pos;
	scene.createEntity("test_cube", cube, physics);

	V3 sphere_pos(0, 5, 5);
	V3 sphere_size(1, 1, 1);
	Colour c2 = COLOUR::MAROON;

	Mesh sphere("C://Users//olive//OneDrive - York College//College Work//Computer Science//AS2//2nd Year Project//Coursework//Program//FINAL//Final Version (not uploaded)//3DEnvironmentCreator-OP//models//car.obj", sphere_size, c);

	PhysicsData sphere_physics;
	physics.position = sphere_pos;
	scene.createEntity("test_sphere", sphere, sphere_physics);

	*/

	// start engine loop
	game.loop();

	// cleanup
	game.exit();
}