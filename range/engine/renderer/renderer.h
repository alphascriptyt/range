#pragma once

#include "SDL.h"

#include "vec2D.h"
#include "camera.h"
#include "mat4d.h"
#include "mathutils.h"
#include "triangle.h"
#include "lightsource.h"
#include "colour.h"

#include <vector>

// renderer class to wrap SDL_Renderer and my own rendering
class Renderer {
private:
	// rendering buffers
	SDL_Surface* windowSurface = nullptr;	// SDL render target
	uint32_t* pixelBuffer = nullptr;		// pixel buffer
	std::vector<float> depthBuffer;			// buffer for depth of pixels

	int32_t bufferPitch = 0;			// store the number of bytes of single row in buffer
	int32_t bufferLength = 0;			// store the length of the buffer

	M4 projectionMatrix;

	// renderer settings - change?
	int FOV = 90;
	float halfTanFOV = tan(toRadians(FOV) / 2);				// precalculate for projection matrix
	float nearPlane = 0.1;									// inside viewers head
	float farPlane = 20;									// outside render vision - default depth buffer value
	int WN_WIDTH = 800;										// window width
	int WN_HEIGHT = 600;									// window height

	// internal initialization methods
	bool initBuffers(); // fill the buffers with default values
	void createProjectionMatrix(); // pre-calculate the projection matrix

	// 3D rendering methods
	void viewTransform(V3& v);
	void rotateMeshFace(V3& v1, V3& v2, V3& v3, V3& pos, float pitch, float yaw);

	bool testAndSetDepth(int pos, float z);				// depth testing
	bool backfaceCull(V3& v1, V3& v2, V3& v3);			// cull backfaces

	float findSignedDistance(V3& v, V3& plane_normal, V3& plane_point);			// clipping helper
	void clipTriangle(V3& v1, V3& v2, V3& v3, std::vector<Triangle>& clipped);	// clip triangles

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
	Camera camera;						// Renderer camera

	// constructors
	Renderer();
	Renderer(int w, int h);

	// rendering API
	bool init();	// setup renderer
	void render();	// actually render the scene

	// 2D primitive drawing
	void drawScanLine(int x1, int x2, int y, int colour, float z1 = -1, float z2 = -1, bool fill = true); // basic line drawing for triangle methods with depth testing
	void drawRectangle(int x1, int y1, int x2, int y2, int colour);
	void drawTriangle(V2& v1, V2& v2, V2& v3, int colour, bool fill = true); // calls other drawing functions - TODO: should take in corresponding colours for each vertex?
	void setBackgroundColour(int colour = 0); // fill background with colour (default = black)
};