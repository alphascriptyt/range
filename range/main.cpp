#define SDL_MAIN_HANDLED // to allow for my own main function

#include "engine/engine.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/colour.h"
#include "engine/renderer/presets.h"
#include "engine/physics/physics.h"

#include <iostream>

int main(int argc, char** argv) {

	// TODO: need to figure out how to get the game to interact with the engine and how to choose what text should be drawn etc.
	// TODO: collision detection, I believe I should calculate bounding boxes of meshes initially and test them against the camera position,
	//			also cache the previous camera location and reset if collided with collision mesh.
	//		https://gamedev.stackexchange.com/questions/66193/how-to-detect-collision-between-a-camera-and-a-mesh
	// should move the camera/player then.
	// see: https://www.youtube.com/watch?v=wVhSQHKvBW4

	Engine engine;

	// startup the window
	if (!engine.setup(960, 720)) {
		printf("Failed to startup.\n");
		return -1;
	}
	
	// keep engine loading of scenes so i can use 3denvcreator to make map?
	//engine.loadScene("C://Users//olive//OneDrive - York College//College Work//Computer Science//AS2//2nd Year Project//Coursework//Program//FINAL//Final Version (not uploaded)//3DEnvironmentCreator-OP//traffic.txt");

	// create this map?
	// https://www.google.com/search?q=quake+map&tbm=isch&ved=2ahUKEwj3oKjr_uz4AhUJZ_EDHXREAagQ2-cCegQIABAA&oq=quake+map&gs_lcp=CgNpbWcQAzIFCAAQgAQyBQgAEIAEMgUIABCABDIFCAAQgAQyBggAEB4QBTIGCAAQHhAFMgYIABAeEAgyBggAEB4QCDIGCAAQHhAIMgYIABAeEAg6BAgAEBg6BAgjECc6CAgAEIAEELEDOgsIABCABBCxAxCDAToECAAQQzoECAAQAzoHCAAQsQMQQ1DjN1jTPWDpPmgAcAB4AIABvgGIAc8FkgEDOS4xmAEAoAEBqgELZ3dzLXdpei1pbWfAAQE&sclient=img&ei=UxPKYrerM4nOxc8P9IiFwAo&bih=658&biw=1523&rlz=1C1CHBF_en-GBGB914GB914#imgrc=WyNLVBQ3CtEPkM

	V3 mesh_pos(0, -3, 5);
	V3 mesh_size(20, 0.1, 20);
	Colour c(255, 20, 65);
	Mesh mesh_floor(Cube::vertices, Cube::faces, mesh_pos, mesh_size, c);
	Scene scene;
	std::string name = "floor";
	PhysicsData physics;
	scene.createSceneObject(name, mesh_floor, physics);

	/*
	V3 light_pos(0, 3, 0);
	LightSource source(light_pos, COLOUR::BLUE, 100);
	V3 size(0.1, 0.1, 0.1);
	Mesh source_mesh(Cube::vertices, Cube::faces, light_pos, size, COLOUR::BLUE);
	*/
	// start engine loop
	engine.loop();

	// cleanup
	engine.exit();
}