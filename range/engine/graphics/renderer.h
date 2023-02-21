#pragma once

#include "SDL.h"
#include "SDL_ttf.h"

#include "vec2D.h"
#include "camera.h"
#include "mat4d.h"
#include "mathutils.h"
#include "triangle3D.h"
#include "lightsource.h"
#include "colour.h"
#include "frustumculling.h"
#include "../scene.h"
#include "rendersurface.h"

#include <vector>
#include <string>

// TODO: static methods begin and end for rendering? or just methods?

// renderer class to wrap all rendering
class Renderer {
private:
	SDL_Surface* texture = nullptr;

	// TEMP
	TTF_Font* font = nullptr;
	int currentVertexTemp;
	int currentTriTemp;
	int currentTriVertexTemp;
	int currentMeshTemp;
	
	M4 projectionMatrix;
	M4 viewMatrix; 
	M4 modelViewMatrix; // TEMP: Testing
	ViewFrustum viewFrustum;

	// renderer settings - change?
	int FOV = 90;											// horizontal FOV
	float nearPlane = 0.1f;									// inside viewers head
	float farPlane = 100;									// outside render vision - default depth buffer value
	int WN_WIDTH = 960;										// window width
	int WN_HEIGHT = 720;									// window height

	// internal setup methods
	void createProjectionMatrix();	// pre-calculate the projection matrix

	// 3D rendering methods
	void viewTransform(V3& v);
	void rotateMeshFace(V3& v1, V3& v2, V3& v3, V3& pos, float pitch, float yaw);

	V2 project(V3& rotated);

	// lighting methods
	float applyPointLighting(V3& v1, V3& v2, V3& v3, LightSource& light);
	void applyLighting(V3& v1, V3& v2, V3& v3, Colour& base_colour);
	float calculateDiffusePart(V3& v, V3& n, V3& light_pos, float a, float b);
	void getVertexColours(V3& v1, V3& v2, V3& v3, Colour& base_colour, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3);
	
	// other 3D methods
	void drawLine3D(V3& v1, V3& direction, float length, Colour& colour);

	// internal 2D primitive helper methods
	void drawFlatBottomTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3); // calls other drawing functions
	void drawFlatTopTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3); // calls other drawing functions
	
public:
	// TEMP: HERE
	RenderSurface renderSurface;							// contains all buffers to write to 

	// initialization
	SDL_Window* window = nullptr;		// SDL window 
	Camera* camera = nullptr;			// Engine camera

	// constructors
	Renderer();
	Renderer(int w, int h);

	// rendering API
	bool init(const std::string& window_name);	// setup renderer
	void renderScene(Scene* scene);				// render the scene to the buffer
	void display();								// render the buffer to the screen
	void cleanup();								// cleanup all renderer resources and SDL instances

	// 2D primitive drawing
	void drawLine(V2& v1, V2& v2, Colour colour);
	void drawScanLine2(int x1, int x2, int y, Colour colour_v1, Colour colour_v2, float z1 = -1, float z2 = -1); // basic line drawing for triangle methods with depth testing
	void drawScanLine(int x1, int x2, int y, Colour colour_v1, Colour colour_v2, float z1 = -1, float z2 = -1); // basic line drawing for triangle methods with depth testing
	void drawRectangle(int x1, int y1, int x2, int y2, int colour);
	void drawTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3); // calls other drawing functions
	void setBackgroundColour(int colour = 0); // fill background with colour (default = black)

	// debugging tools
	void drawNormal(V3& origin, V3& normal);

	bool temp = false;
};