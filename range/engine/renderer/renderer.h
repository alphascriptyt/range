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

#include <vector>
#include <string>

// renderer class to wrap SDL_Renderer and my own rendering
class Renderer {
private:
	// TEMP
	TTF_Font* font = nullptr;

	// rendering buffers
	SDL_Surface* windowSurface = nullptr;	// SDL render target
	uint32_t* pixelBuffer = nullptr;		// pixel buffer
	std::vector<float> depthBuffer;			// buffer for depth of pixels

	int32_t bufferPitch = 0;				// store the number of bytes of single row in buffer
	int32_t bufferLength = 0;				// store the length of the buffer

	M4 projectionMatrix;
	ViewFrustum viewFrustum;

	// renderer settings - change?
	int FOV = 90;
	float halfTanFOV = tan(toRadians(FOV) / 2);				// precalculate for projection matrix
	float nearPlane = 0.1;									// inside viewers head
	float farPlane = 20;									// outside render vision - default depth buffer value
	int WN_WIDTH = 960;										// window width
	int WN_HEIGHT = 720;									// window height

	// internal setup methods
	bool initBuffers();				// fill the buffers with default values
	void createProjectionMatrix();	// pre-calculate the projection matrix

	// 3D rendering methods
	void viewTransform(V3& v);
	void rotateMeshFace(V3& v1, V3& v2, V3& v3, V3& pos, float pitch, float yaw);

	bool testAndSetDepth(int pos, float z);				// depth testing
	bool backfaceCull(V3& v1, V3& v2, V3& v3);			// cull backfaces

	V2 project(V3& rotated);

	// lighting methods
	float applyPointLighting(V3& v1, V3& v2, V3& v3, LightSource& light);
	void applyLighting(V3& v1, V3& v2, V3& v3, Colour& base_colour);
	
	// other 3D methods
	void drawLine3D(V3& v1, V3& direction, float length);

	// internal 2D primitive helper methods
	void drawFlatBottomTriangle(V2& v1, V2& v2, V2& v3, int colour, bool fill = true); // calls other drawing functions
	void drawFlatTopTriangle(V2& v1, V2& v2, V2& v3, int colour, bool fill = true); // calls other drawing functions

public:
	// initialization
	SDL_Window* window = nullptr;		// SDL window 
	Camera* camera = nullptr;			// Engine camera

	// constructors
	Renderer();
	Renderer(int w, int h);

	// rendering API
	bool init();						// setup renderer
	void renderScene(Scene* scene);		// actually render the scene
	void cleanup();						// cleanup all renderer resources and SDL instances

	// 2D primitive drawing
	void drawScanLine(int x1, int x2, int y, int colour, float z1 = -1, float z2 = -1, bool fill = true); // basic line drawing for triangle methods with depth testing
	void drawRectangle(int x1, int y1, int x2, int y2, int colour);
	void drawTriangle(V2& v1, V2& v2, V2& v3, int colour, bool fill = true); // calls other drawing functions - TODO: should take in corresponding colours for each vertex?
	void setBackgroundColour(int colour = 0); // fill background with colour (default = black)

	// text drawing methods
	void renderText(char* text, Colour& colour, int x, int y);
};