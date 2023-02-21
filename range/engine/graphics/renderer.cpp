#include "SDL.h"
#include "SDL_image.h"
#include "mesh.h"
#include "renderer.h"
#include "mat4D.h"
#include "vectormaths.h"
#include "triangle3D.h"
#include "lightsource.h"

#include <cmath>
#include <iostream>
#include <limits>

// setup
Renderer::Renderer() {
	viewFrustum = ViewFrustum(nearPlane);
}

Renderer::Renderer(int w, int h) {
	WN_WIDTH = w;
	WN_HEIGHT = h;
	viewFrustum = ViewFrustum(nearPlane);
}

void Renderer::createProjectionMatrix() {
	// NOTE: https://www.youtube.com/watch?v=8bQ5u14Z9OQ
	// NOTE: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/#the-projection-matrix

	// pre calculate the projection matrix once at the start as it shouldn't change
	float aspect_ratio = WN_WIDTH / (float)WN_HEIGHT;
	float half_tan_fov = 1.0f / tan(toRadians(FOV) / 2.0f); // TODO: rename? distance to near plane?

	
	/* NOTE: Old row-major perspective projection matrix?
	projectionMatrix.setRow(0, 1 / half_tan_fov, 0, 0, 0);
	projectionMatrix.setRow(1, 0, aspect_ratio / half_tan_fov, 0, 0);
	projectionMatrix.setRow(2, 0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), 1);	
	projectionMatrix.setRow(3, 0, 0, (2 * nearPlane * farPlane) / (nearPlane - farPlane), 0);
	*/
	
	// column-major perspective projection matrix
	// we scale x by the aspect ratio as we are using horizontal FOV
	// [0,0] and [1,1] scale the x,y by the distance to the nearPlane
	/*
	projectionMatrix.setRow(0, half_tan_fov / aspect_ratio, 0, 0, 0);
	projectionMatrix.setRow(1, 0, half_tan_fov, 0, 0);
	projectionMatrix.setRow(2, 0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), (2 * nearPlane * farPlane) / (nearPlane - farPlane));
	projectionMatrix.setRow(3, 0, 0, -1, 0);
	*/

	
	

	// row major	
	projectionMatrix.setRow(0, -half_tan_fov / aspect_ratio, 0, 0, 0); // made negative so x is correct? right = positive
	projectionMatrix.setRow(1, 0, half_tan_fov, 0, 0);
	//projectionMatrix.setRow(2, 0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), -1);
	//projectionMatrix.setRow(3, 0, 0, (2 * nearPlane * farPlane) / (nearPlane - farPlane), 0);
	//projectionMatrix.setRow(2, 0, 0, (nearPlane + farPlane) / (farPlane - nearPlane), -1);
	//projectionMatrix.setRow(3, 0, 0, -(2 * nearPlane * farPlane) / (farPlane - nearPlane), 0);
	projectionMatrix.setRow(2, 0, 0, farPlane / (farPlane - nearPlane), -1);
	projectionMatrix.setRow(3, 0, 0, -(nearPlane * farPlane) / (farPlane - nearPlane), 0);


	 // TODO: USE OLD CODE
	// TEMP: OLD MATRIX
	projectionMatrix.setRow(0, -1 / half_tan_fov, 0, 0, 0);
	projectionMatrix.setRow(1, 0, aspect_ratio / half_tan_fov, 0, 0);
	//projectionMatrix.setRow(2, 0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), -1);
	//projectionMatrix.setRow(3, 0, 0, (2 * nearPlane * farPlane) / (nearPlane - farPlane), 0);


	/*
	projection_matrix.setRow(0, 1 / halfTanFOV, 0, 0, 0);
	projection_matrix.setRow(1, 0, aspectRatio / halfTanFOV, 0, 0);
	projection_matrix.setRow(2, 0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), -1);
	projection_matrix.setRow(3, 0, 0, (2 * nearPlane * farPlane) / (nearPlane - farPlane), 0);
	*/
	


	/*
	projectionMatrix.setRow(0, -half_tan_fov / aspect_ratio, 0, 0, 0); // made negative so x is correct? right = positive
	projectionMatrix.setRow(1, 0, half_tan_fov, 0, 0);
	projectionMatrix.setRow(2, 0, 0, farPlane / (farPlane - nearPlane), 0);
	projectionMatrix.setRow(3, 0, 0, -(nearPlane * farPlane) / (farPlane - nearPlane), 1);


	projectionMatrix.setRow(0, -half_tan_fov / aspect_ratio, 0, 0, 0); // made negative so x is correct? right = positive
	projectionMatrix.setRow(1, 0, half_tan_fov, 0, 0);
	projectionMatrix.setRow(2, 0, 0, -farPlane / (farPlane - nearPlane), -1);
	projectionMatrix.setRow(3, 0, 0, -(nearPlane * farPlane) / (farPlane - nearPlane), -1);
	*/

	// TODO: having the [3][3] as 1 sort of.. fixes the clipping issue? SO it has to be to do with w?
}

bool Renderer::init(const std::string& window_name) {
	// create window
	window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WN_WIDTH, WN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!window) {
		printf("Failed to create window - %s\n", SDL_GetError());
		return false;
	}

	// initialize SDL_ttf
	if (TTF_Init() == -1) {
		printf("Failed to initialize SDL_ttf - %s\n", TTF_GetError());
		return false;
	}

	// TEMP: load the global font
	font = TTF_OpenFont("C:/Users/olive/source/repos/range/range/engine/res/font/louisgeorgecafe_base.ttf", 28);

	if (!font) {
		printf("Failed to open font - %s\n", TTF_GetError());
		return false;
	}

	// check for failures creating the rendersurface
	if (!renderSurface.init(window, WN_WIDTH, WN_HEIGHT, COLOUR::WHITE.toInt(), nearPlane)) {
		return false;
	}

	IMG_Init(IMG_INIT_JPG);
	texture = IMG_Load("C:/Users/olive/source/repos/range/range/wood.jpg");
	if (!texture) {
		std::cout << "TEMP: Texture failed to load.\n" << IMG_GetError();
	}

	// create projection matrix
	createProjectionMatrix();
	
	// create the viewpoint lightsource (has to be created first so we can access it easily)
	Scene::scenes[0]->createLightSource("viewpoint", camera->physics.position, COLOUR::WHITE, 10);

	return true;
}

// 2D primitive drawing
void Renderer::drawLine(V2& v1, V2& v2, Colour colour) {
	// CREDITS: https://github.com/joshb/linedrawing/blob/master/Rasterizer.cpp
	// TODO: optimise this and write it more in my style.

	int colour_int = colour.toInt();

	float xdiff = (v2.x - v1.x);
	float ydiff = (v2.y - v1.y);
	
	if (fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;

		// set xmin to the lower x value given
		// and xmax to the higher value
		if (v1.x < v2.x) {
			xmin = v1.x;
			xmax = v2.x;
		}
		else {
			xmin = v2.x;
			xmax = v1.x;
		}

		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		for (float x = xmin; x <= xmax; x += 1.0f) {
			float y = v1.y + ((x - v1.x) * slope);
			
			if (x >= 0 && x < WN_WIDTH && y >= 0 && y < WN_HEIGHT) renderSurface.setPixel(x, y, colour_int);
		}
	}
	else {
		float ymin, ymax;

		// set ymin to the lower y value given
		// and ymax to the higher value
		if (v1.y < v2.y) {
			ymin = v1.y;
			ymax = v2.y;
		}
		else {
			ymin = v2.y;
			ymax = v1.y;
		}

		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for (float y = ymin; y <= ymax; y += 1.0f) {
			float x = v1.x + ((y - v1.y) * slope);
			if (x >= 0 && x < WN_WIDTH && y >= 0 && y < WN_HEIGHT) renderSurface.setPixel(x, y, colour_int);
			
		}
	}
}

void Renderer::drawScanLine2(int x1, int x2, int y, Colour colour_v1, Colour colour_v2, float z1, float z2) {
	// TODO: this method is extremely slow. About 70% execution time is spent here, obviously lots will be here but that's too much.

	// if y is off screen or y is below screen, ignore
	if (y > (WN_HEIGHT - 1) || y < 0) { return; }

	// ignore lines starting outside the x boundaries
	if (x1 > WN_WIDTH - 1 || x2 < 0) { return; }

	// precalculate values
	float dx = x2 - x1;
	Colour dc = colour_v2 - colour_v1;

	// clip point to left side
	if (x1 < 0) {
		colour_v1 = colour_v1 + (dc * ((-x1) / dx));
		x1 = 0;
	}

	// clip point to right side
	if (x2 > WN_WIDTH - 1) {
		colour_v2 = colour_v2 - (dc * ((x2 - WN_WIDTH) / dx));
		x2 = WN_WIDTH - 1;
	}

	// scale x in terms of y
	int row_offset = WN_WIDTH * y;
	x1 += row_offset;
	x2 += row_offset;

	// fill line in array

	// calculate depth increment
	float z_increment = (z2 - z1) / dx;

	
	// calculate colour increment
	Colour colour_step = dc / dx;
	Colour colour = colour_v1;
	/*
	float r = colour.r * 255;
	float g = colour.g * 255;
	float b = colour.b * 255;

	float r_step = colour_step.r * 255;
	float g_step = colour_step.g * 255;
	float b_step = colour_step.b * 255;
	*/
	// TODO: Look for bisqwit video that uses ints as colours?
	// maybe this? https://bisqwit.iki.fi/jutut/kuvat/programming_examples/polytut/04-generic.cc

	// NOTE: converting to int every frame ruins fps.

	//float zt = z1;

	Colour c1z = colour_v1 * z1;
	Colour c2z = colour_v2 * z2;
	
	Colour c_step = (c2z - c1z) / dx;

	float dz = (z2 - z1) / dx;

	float z = z1;

	// render the scanline
	for (int x = x1; x <= x2; ++x) {
		// look into: https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf

		float s = (x - x1) / (float)x2;

		//float zt = (z1 + s * (z2 - z1));
		//if (x1 == x2) std::cout << zt << std::endl;

		//colour = (c1z + cd * s) * zt;

		//std::cout << s << " " << zt << std::endl;
		
		//renderSurface.setIndex(x, ((c1z * (1 - s) + (c2z * s)) * z).toInt(), z);
		renderSurface.setIndex(x, (colour / z).toInt(), z);
	

		z += dz;
		colour += c_step;

	}

}

void Renderer::drawScanLine(int x1, int x2, int y, Colour colour_v1, Colour colour_v2, float z1, float z2) {
	// TODO: this method is extremely slow. About 70% execution time is spent here, obviously lots will be here but that's too much.
	
	// if y is off screen or y is below screen, ignore
	if (y > (WN_HEIGHT - 1) || y < 0) { return; }

	// ignore lines starting outside the x boundaries
	if (x1 > WN_WIDTH - 1 || x2 < 0) { return; }
	
	// precalculate x2 - x1
	float x2x1 = x2 - x1;
	Colour c2c1 = colour_v2 - colour_v1;
	
	
	// clip point to left side
	if (x1 < 0) {
		colour_v1 = colour_v1 + (c2c1 * ((-x1) / x2x1));
		x1 = 0;
	}
	
	// clip point to right side
	if (x2 > WN_WIDTH - 1) {
		colour_v2 = colour_v2 - (c2c1 * ((x2 - WN_WIDTH) / x2x1));
		x2 = WN_WIDTH - 1;
	}
	
	// scale x in terms of y
	int row_offset = WN_WIDTH * y;
	x1 += row_offset;
	x2 += row_offset;
	
	// fill line in array

	// calculate depth increment
	float z_increment = (z2 - z1) / x2x1;

	// calculate colour increment
	//sColour colour_step = (colour_v2 - colour_v1) / x2x1;
	Colour colour_step = c2c1 / x2x1;
	Colour colour = colour_v1;

	float r = colour.r * 255;
	float g = colour.g * 255;
	float b = colour.b * 255;

	float r_step = colour_step.r * 255;
	float g_step = colour_step.g * 255;
	float b_step = colour_step.b * 255;

	// TODO: Look for bisqwit video that uses ints as colours?
	// maybe this? https://bisqwit.iki.fi/jutut/kuvat/programming_examples/polytut/04-generic.cc

	// NOTE: converting to int every frame ruins fps.

//	std::cout << z1 << " " << z2 << std::endl;

	// render the scanline
	for (; x1 <= x2; ++x1) {
		// set the pixel via index

		float z = 1.0f / z1;

		//renderSurface.setIndex(x1, colour.toInt(), z1); // TODO: I think this is actually the slow part!! look into this.
		//renderSurface.setIndex(x1, ((int)(r * z) << 16) | ((int)(g * z) << 8) | ((int)(b * z)), z1); // TODO: I think this is actually the slow part!! look into this.
		renderSurface.setIndex(x1, ((int)(r) << 16) | ((int)(g) << 8) | ((int)(b)), z1); // TODO: I think this is actually the slow part!! look into this.
		
		
		// increment depth and colour
		z1 += z_increment;
		//colour += colour_step;

		r += r_step;
		g += g_step;
		b += b_step;
		//colour += colour_step;	

		
	}
}

void Renderer::drawRectangle(int x1, int y1, int x2, int y2, int colour) {
	// ensure coordinates go from left to right
	if (x1 > x2) {
		std::swap(x1, x2);
	}

	// ensure coordinates go from top to bottom
	if (y1 > y2) {
		std::swap(y1, y2);
	}

	// draw the pixels
	for (int y = y1; y < y2; ++y) {
		// scale x in terms of y
		int start_x = x1 + (WN_WIDTH * y);
		int end_x = x2 + (WN_WIDTH * y);

		for (int x = start_x; x < end_x; ++x) {
			// set the pixel
			renderSurface.setIndex(x, colour);
		}
	}

}

void Renderer::drawFlatBottomTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3) {
	// here we draw each scanline from the highest vertex v1
	if (v2.x > v3.x) { std::swap(v2, v3); std::swap(colour_v2, colour_v3); } // ensure v3 is bigger than v2

	// precalculate vars
	float y2_sub_y1 = (v2.y - v1.y);
	float y3_sub_y1 = (v3.y - v1.y);

	// TODO: relabel all m1, m2... as they're not actually dx/dy they're delta values

	// calculate dx/dy for each triangle side
	float m1 = (v2.x - v1.x) / y2_sub_y1;
	float m2 = (v3.x - v1.x) / y3_sub_y1;

	// calculate dz/dy for each triangle side
	float mz1 = (v2.w - v1.w) / y2_sub_y1;
	float mz2 = (v3.w - v1.w) / y3_sub_y1;

	// set initial start and end x of line
	float start_x = v1.x;
	float end_x = v1.x;

	// set initial start and end depth of line
	float start_z = v1.w;
	float end_z = v1.w;

	// calculate colour gradients for each triangle side
	Colour mc1 = (colour_v2 - colour_v1) / y2_sub_y1;
	Colour mc2 = (colour_v3 - colour_v1) / y3_sub_y1;

	// calculate colours for start and end of lines
	Colour start_colour = colour_v1;
	Colour end_colour = colour_v1;

	// step through y to calculate start and end of scanline
	for (int y = v1.y; y <= v2.y; ++y) {
		// draw the scanline
		//drawScanLine(start_x, end_x, y, colour, start_z, end_z, fill);
		drawScanLine(start_x - 0.5f, end_x + 0.5f, y, start_colour, end_colour, start_z, end_z);

		// for each increment of y, step through x by dx/dy
		start_x += m1;
		end_x += m2;

		// step through z by dz/dy
		start_z += mz1;
		end_z += mz2;

		// step through colour by dc/dy
		start_colour += mc1;
		end_colour += mc2;
	}
}

void Renderer::drawFlatTopTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3) {
	if (v1.x > v2.x) { std::swap(v1, v2); std::swap(colour_v1, colour_v2); } // ensure v3 is bigger than v2

	float dy = (v3.y - v1.y); // v3.y == v2.y

	// calculate dx/dy for each triangle side
	float m1 = (v3.x - v1.x) / dy;
	float m2 = (v3.x - v2.x) / dy;

	// calculate dz/dy for each triangle side
	float mz1 = (v3.w - v1.w) / dy;
	float mz2 = (v3.w - v2.w) / dy;

	// set initial start and end x of line
	float start_x = v3.x;
	float end_x = v3.x;

	// set initial start and end depth of line
	float start_z = v3.w;
	float end_z = v3.w;

	// calculate colour gradients for each triangle side
	Colour mc1 = (colour_v3 - colour_v1) / dy;
	Colour mc2 = (colour_v3 - colour_v2) / dy;

	// calculate colours for start and end of lines
	Colour start_colour = colour_v3;
	Colour end_colour = colour_v3;

	// step through y to calculate start and end of scanline
	for (int y = v3.y; y >= v1.y; y--) { // TESTING
		// draw the scanline
		//drawScanLine(start_x, end_x, y, colour, start_z, end_z, fill);
		drawScanLine(start_x - 0.5f, end_x + 0.5f, y, start_colour, end_colour, start_z, end_z);

		// step through x by dx/dy
		start_x -= m1;
		end_x -= m2;

		// step through z by dz/dy
		start_z -= mz1;
		end_z -= mz2;

		// step through colour by dc/dy
		start_colour -= mc1;
		end_colour -= mc2;
	}
}

void Renderer::drawTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3) {
	/*
	What we need to do:
		- Use barycentric coordinates to compute perspective correct values for each pixel.

	Steps:
		- Compute barycentric coordinates for the triangle
		- Compute the depth of the pixel
		- Multiply vertex attributes (colour) by the interpolated depth to correct for perspective
	
	https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf

	*/

	// sort vertices in ascending order
	if (v1.y > v2.y) { std::swap(v1, v2); std::swap(colour_v1, colour_v2); } // ensure v2 is bigger than v1
	if (v1.y > v3.y) { std::swap(v1, v3); std::swap(colour_v1, colour_v3); } // ensure v3 is bigger than v1
	if (v2.y > v3.y) { std::swap(v2, v3); std::swap(colour_v2, colour_v3); } // ensure v3 is bigger than v2

	drawLine(v1, v2, COLOUR::WHITE);
	drawLine(v2, v3, COLOUR::WHITE);
	drawLine(v1, v3, COLOUR::WHITE);

	// check if triangles already has a flat top/bottom 
	if (v1.y == v2.y) { drawFlatTopTriangle(v1, v2, v3, colour_v1, colour_v2, colour_v3); return; }
	if (v2.y == v3.y) { drawFlatBottomTriangle(v1, v2, v3, colour_v1, colour_v2, colour_v3); return; }

	// calculate v4's x (opposite side from v2)
	float t = (v2.y - v1.y) / (v3.y - v1.y);
	float x = v1.x + t * (v3.x - v1.x); // lerp for x

	// TODO: v4's x is sometimes extremely far to the left when it shouldn't be? because v3.x is extremely small?? the vertex
	//		is essentially ended up stupidly far to the left, maybe due to FOV? something wrong with projection?

	// calculate interpolated depth
	// TODO: figure out if this interpolation is correct
	float w = v1.w + t * (v3.w - v1.w);


	//std::cout << " v1.w: " << v1.w;
	//std::cout << " v2.w: " << v2.w;
	//std::cout << " v3.w: " << v3.w;
	//std::cout << " w: " << w << std::endl;
	 
	if (x < -3000) { 
		//std::cout << "v1.x: " << v1.x << std::endl << "v3.x: " << v3.x << std::endl << "v3.y: " << v3.y << std::endl;
	}

	V2 v4(x, v2.y, w);
	std::cout << v1.x << std::endl;
	Colour colour_v4 = colour_v1 + (colour_v3 - colour_v1) * t;
	


	//drawLine(v2, v4, COLOUR::WHITE);
	//drawScanLine(v2.x, v4.x, v2.y, COLOUR::WHITE, COLOUR::WHITE);


	// draw split triangles
	drawFlatBottomTriangle(v1, v2, v4, colour_v1, colour_v2, colour_v4);
	drawFlatTopTriangle(v2, v4, v3, colour_v2, colour_v4, colour_v3);
}

// rendering methods
void Renderer::viewTransform(V3& v) {
	// translate scene with respect to camera
	v -= camera->physics.position;
}

V2 Renderer::project(V3& rotated) {
	/*
	- https://www.youtube.com/watch?v=8bQ5u14Z9OQ
	- https://stackoverflow.com/questions/25584667/why-do-i-divide-z-by-w-in-a-perspective-projection-in-opengl

	The projectionMatrix converts view space -> Normalised Device Coordinates (NDC) Space
	NDC ranges from (-1, -1, 0) to (1, 1, 1) - OPENGL maps z from -1, 1 but I like direct3D 0,1

	we then divide x,y by z to give perspective (smaller futher away)

	x,y,z get mapped to these values and z gets copied to w?

	when w != 1, the vector is in homogeneous coordinates, so we must scale it down
	so we divide everything by w to uniformally scale it down, removing the extra 4D component
	*/


	// convert a 4D matrix to 2D pixel coordinates
	V3 projected = rotated * projectionMatrix;
	
	// scale to screen
	V2 pixel_coords;

	pixel_coords.w = -1.0f / projected.w;
	
	//std::cout << "w: " << pixel_coords.w << std::endl;

	
	projected.x /= projected.w;
	projected.y /= projected.w;
	projected.z /= projected.w;
	



	// NDC seems to be correct?

	// TODO: THE WHOLE ISSUE IS WITH THE W, WHEN MESSING AROUND WITH OTHER PROJECTION MATRICES IT DIDN'T DO IT.

	// TODO: THE ISSUE IS WHEN A VERTEX IS BEHIND THE CAMERA, ITS W GETS SET TO -10 WHICH GIVES STUPID VALUES
	//		 when we clip, z gets set to 0.1, then 1 / 0.1 = 10, this is a huge value for w which isn't right 
	//		 as the depth is only 0.1? This means the colour seems super high when depth correcting
	//			- as soon as we clip and get that near plane value we get stupid high values
	//		BUT.... Is this the error???
	//			when w = -9.747797, projected =  5.566510 0.136983 0.949509 -0.102587 which is a normal value...
	//			so why does w = 10 screw us up so bad? do more investigating into what is causing the crazy values...

	// TODO: actually think about how it works, like why do neara plane values go so big and what should they be instead?

	// Z is the distance from the near clipping plane to the fragment.W is the distance from the camera to the fragment.
	//	TODO: if this is the case then z is wrong, as z should be 0? 
	//http://www.bluevoid.com/opengl/sig99/advanced99/notes/node28.html

	// TODO: look into clipping maybe? https://gabrielgambetta.com/computer-graphics-from-scratch/11-clipping.html

	// TODO: z/w should be used for depth buffer?

	/*
	w: -9.999998
	 -10.016663 7.434866 1.000000 -0.100000
	w: -9.999998
	 -9.963309 5.439899 1.000000 -0.100000
	w: -1.344991
	 -1.102080 -0.450524 -0.732735 -0.743500
	v1.x: -65.331192
	v3.x: -5770.664062
	v3.y: 3036.551758
	-4327.241211
	
	*/

	// perspective divide
//	projected *= pixel_coords.w;

	// TODO: when clipped, the z value is always 1 and w is 0.1

	// z component of V3 is copied to w
	// divide by w, to correct projection for perspective
	// so shape gets smaller as it gets further away
	if (projected.w != 0) {
	//	projected.x /= projected.w;
		//projected.y /= projected.w;
		//projected.z /= projected.w;
	}

//	projected.print();

	// center in screen and scale
	pixel_coords.x = (projected.x + 1) * 0.5f * WN_WIDTH;
	pixel_coords.y = (projected.y + 1) * 0.5f * WN_HEIGHT;
	
	// precalculate 1/w for per pixel interpolation
	// TODO: This doesn't feel 100% right.
	// TODO: Back to lerping I guess.
	//pixel_coords.w = -1.0f / projected.w; // TODO: making z positive means depth test is backwards
	
	//printf("w: %f, z: %f\n", pixel_coords.w, rotated.z);

	//std::cout << "1/w: " << pixel_coords.w << std::endl;

	// TODO: fix? W ends up being out of the range (near 10?) if the coordinate is behind the camera
	
	return pixel_coords;
}

void Renderer::setBackgroundColour(int colour) {
	//std::fill(pixelBuffer, pixelBuffer + bufferLength, colour);
}

void Renderer::drawNormal(V3& origin, V3& normal) {
	
}

void Renderer::rotateMeshFace(V3& v1, V3& v2, V3& v3, V3& pos, float pitch, float yaw) {
	// translate vertices to origin for rotation
	V3 tv1 = v1 - pos;
	V3 tv2 = v2 - pos;
	V3 tv3 = v3 - pos;

	// rotate vertices
	rotateV3(tv1, pitch, yaw);
	rotateV3(tv2, pitch, yaw);
	rotateV3(tv3, pitch, yaw);

	// translate vertices back to original position
	v1 = pos + tv1;
	v2 = pos + tv2;
	v3 = pos + tv3;
}

void Renderer::drawLine3D(V3& v1, V3& direction, float length, Colour& colour) {
	// calculate the end point
	V3 v2 = v1 + direction * length;

	// project the start and end coordinates to 2D
	V2 start = project(v1);
	V2 end = project(v2);

	if (start.x > end.x) { std::swap(start, end); }

	drawLine(start, end, colour);
}

float Renderer::applyPointLighting(V3& v1, V3& v2, V3& v3, LightSource& light) {
	// Point lighting is lighting that comes from a point and goes
	// in all directions, here I am just using the diffuse lighting
	// for simplicity. It is calculated from the cosine of the 
	// angle that the light hits the surface which the dot product
	// gives.

	// calculate the lines between vertices
	V3 line1 = v2 - v1;
	V3 line2 = v3 - v1;

	// calculate the face normal
	V3 normal = vectorCrossProduct(line1, line2);
	normal.normalize();

	// convert the light to camera space
	V3 L = light.position;

	// apply view transform
	viewTransform(L);

	// apply camera rotation
	rotateV3(L, camera->pitch, camera->yaw);

	// create vertex in the middle to get average diffuse factor TODO: SHOULD CALCULATE FOR EACH VERTEX AND LERP THEM?
	V3 v4 = (v1 + v2 + v3) / 3.0f;
	
	//v4 = v1;
	// calculate the direction of the light to the vertex
	V3 light_direction = L - v4;
	float light_distance = light_direction.size();
	light_direction.normalize();

	// if we don't normalize we are essentially taking the distance into equation?

	// ISSUE:
	// i think the issue is that when we get so close to the y coordinate, normalizing makes the x and z huge compared to y
	// this makes it seem like the normals go in the wrong direction.
	// do we need to do per pixel somehow? i'd rather not, but not sure how else

	// coordinates get flipped so we must invert the light direction
	light_direction *= -1;

	// calculate how much the vertex is lit
	
	float diffuse_factor = vectorDotProduct(light_direction, normal); // diffuse factor gets bigger the further away???
	
	//std::cout << "light_dir "; light_direction.print();
	//std::cout << "normal "; normal.print();
	
	// TODO: diffuse factor is extremely low when close
	//  lighting messes up close up.

	// determine if the face is lit by the lightsource
	
	if (diffuse_factor < 0.0f) {
		//std::cout << abs(diffuse_factor) << std::endl;
		//diffuse_factor = std::min(abs(diffuse_factor), 1.0f);

		// calculate the light attenuation
		float a = 0.1f / light.strength;
		float b = 0.01f / light.strength;

		// attenuation is correct
		float attenuation = 1.0f / (1.0f + (a * light_distance) + (b * light_distance * light_distance));
		
		//std::cout << light_distance << " " << attenuation << " " << diffuse_factor << " " << diffuse_factor * attenuation << std::endl;
		
		return abs(diffuse_factor * attenuation);
	}
	
	// facing the wrong way?
	return 0.0f;
}

void Renderer::applyLighting(V3& v1, V3& v2, V3& v3, Colour& base_colour) {
	// little hack for viewpoint lighting, set the first
	// lightsource's position to the camera
	LightSource::sources[0]->position = camera->physics.position;
	
	// calculate the material colour parts 
	Colour diffuse_part(0, 0, 0);

	Colour ambient_part = COLOUR::WHITE;
	float ambient_level = 0.1f;
	ambient_part *= ambient_level;

	// loop through each lightsource
	for (int l = 0; l < LightSource::sources.size(); ++l) {
		// get the pointer to the lightsource
		LightSource* light = LightSource::sources[l];

		// only calculate if light enabled
		if (light->enabled) {
			// calculate the diffuse factor for the point light with attenuation
			float dp = applyPointLighting(v1, v2, v3, *light);

			// add to the colour
			diffuse_part += light->colour * dp;
		}
	}

	// calculate the resultant colour off the face
	Colour face_colour = (diffuse_part + ambient_part) * base_colour;
	
	// set the colour of the mesh
	base_colour = face_colour;
}

float Renderer::calculateDiffusePart(V3& v, V3& n, V3& light_pos, float a, float b) {
	// calculate the direction of the light to the vertex
	//V3 light_direction = light_pos - v;
	V3 light_direction = v - light_pos;
		
	// normalise and store size
	float light_distance = light_direction.size();
	//light_direction /= light_distance;

	//drawLine3D(light_pos, light_direction, light_distance, COLOUR::YELLOW);

	light_direction *= -1;

	// calculate how much the vertex is lit
	float diffuse_factor = std::max(0.0f, vectorDotProduct(light_direction, n));

	// attenuation is correct
	float attenuation = 1.0f / (1.0f + (a * light_distance) + (b * light_distance * light_distance));
	float dp = abs(diffuse_factor * attenuation);

	return dp;
}

void Renderer::getVertexColours(V3& v1, V3& v2, V3& v3, Colour& base_colour, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3) {
	// little hack for viewpoint lighting, set the first
	// lightsource's position to the camera
	LightSource::sources[0]->position = camera->physics.position;

	// calculate the material colour parts 
	Colour diffuse_part_v1(0, 0, 0);
	Colour diffuse_part_v2(0, 0, 0);
	Colour diffuse_part_v3(0, 0, 0);

	Colour ambient_part = COLOUR::WHITE;
	float ambient_level = 0.1f;
	//float ambient_contribution = 1 - ambient_level;

	ambient_part *= ambient_level;

	// calculate the lines between vertices
	V3 line1 = v2 - v1;
	V3 line2 = v3 - v1;

	// calculate the face normal
	V3 normal = vectorCrossProduct(line1, line2);
	normal.normalize();

	V3 v4 = (v1 + v2 + v3) / 3.0f;
	//normal = currentNormal;
	//drawLine3D(v4, normal, 3, COLOUR::AQUA);

	

	
	//normal.print();
	//camera->physics.position.print();

	// loop through each lightsource
	for (int l = 0; l < LightSource::sources.size(); ++l) {
		if (l == 0) continue; // TEMP: TESTING

		// get the pointer to the lightsource
		LightSource* light = LightSource::sources[l];

		// only calculate if light enabled
		if (light->enabled) {
			// convert the light to camera space
			
			
			V3 light_pos = light->position * viewMatrix;

			//light_pos.print("light_pos: ");

			// calculate the light attenuation parameters
			float a = 0.1f / light->strength;
			float b = 0.01f / light->strength;

			currentTriVertexTemp = 0;
			float dp = calculateDiffusePart(v1, normal, light_pos, a, b);

			currentTriVertexTemp = 1;

			// add to the colour
			diffuse_part_v1 += light->colour * dp;

			dp = calculateDiffusePart(v2, normal, light_pos, a, b);

			// add to the colour
			diffuse_part_v2 += light->colour * dp;

			dp = calculateDiffusePart(v3, normal, light_pos, a, b);

			// add to the colour
			diffuse_part_v3 += light->colour * dp;

			
		}
	}

	// calculate the resultant colour off the face
	ambient_part = Colour(0, 0, 0);
	colour_v1 = base_colour * (diffuse_part_v1 + ambient_part);
	colour_v2 = base_colour * (diffuse_part_v2 + ambient_part);
	colour_v3 = base_colour * (diffuse_part_v3 + ambient_part);

	//colour_v1.print();
}

void Renderer::renderScene(Scene* scene) {
	// actually render the scene

	// clear render surface
	//renderSurface.clear(0x87CEEB, farPlane);
	renderSurface.clear(0, farPlane);

	
	/*
	V3 v1(-1, -1, 1);
	V3 v2(0, 1, 1);
	V3 v3(1, -1, 1);

	//V3 v1(0, -0.3, 1);
	//V3 v2(0, 0.75, 1);
	//V3 v3(1, -0.25, 1);


	V2 pv1 = project(v1);
	V2 pv2 = project(v2);
	V2 pv3 = project(v3);

	Colour c1 = COLOUR::WHITE;
	Colour c2 = COLOUR::RED;
	Colour c3 = COLOUR::GREEN;

	drawTriangle(pv1, pv2, pv3, c1, c2, c3);
	



	return;
	*/

	viewMatrix = camera->makeViewMatrix();

	std::vector<Triangle3D>triangles = {};
	V3 v1 = V3(0,0,1);
	V3 v2 = V3(1,0,0);
	V3 v3 = V3(0,1,-1);

	v1 = v1 * viewMatrix;
	v2 = v2 * viewMatrix;
	v3 = v3 * viewMatrix;


	Triangle3D tri(v1, v2, v3);
	triangles.push_back(tri);


	V3 plane_normal = V3(0, 0, 1);
	V3 plane_point = V3(0, 0, nearPlane);
	Plane plane(plane_normal, plane_point);

	viewFrustum.clipTrianglesAgainstPlane(triangles, plane);

	/*
	for (Triangle3D& tri : triangles) {
		tri.v1.print("v1: ");
		tri.v2.print("v2: ");
		tri.v3.print("v3: ");
	}
	

	std::cout << "project v2: ";  project(triangles[0].v2).print();
	*/
	//return;

	//viewMatrix.print();
	//return;

	/*
	// vertex coordinate in model space
	V3 test(10, 2, 10);
	V3 normal(0, 1, 0);

	// camera transform
	test = viewMatrix * test;

	V3 light_pos = V3(0, 10, 0);
	light_pos = viewMatrix * light_pos;

	calculateDiffusePart(test, normal, light_pos, 0.1f, 0.01f);
	*/

	// loop through each mesh
	for (int s = 0; s < scene->entities.size(); ++s) {
		Entity* entity = scene->entities[s];
		Mesh* mesh = entity->mesh;

		// store all of the mesh triangles
		std::vector<Triangle3D> triangles;

		currentMeshTemp = s;

		M4 model = entity->makeModelMatrix();
		M4 model_view = model * viewMatrix;
		
		//model_view = viewMatrix; //TEMP: testing

		modelViewMatrix = model_view;

		// loop through each face in mesh
		for (int i = 0; i < mesh->faces.size(); ++i) {
			// get mesh face vertices
			V3 v1 = mesh->vertices[mesh->faces[i][0]];
			V3 v2 = mesh->vertices[mesh->faces[i][1]];
			V3 v3 = mesh->vertices[mesh->faces[i][2]];

			/*
			STEPS:
			Model Space -> World Space:		Model Matrix
			World Space -> Camera Space:	View Matrix
			View Space	-> Clip Space:		Projection Matrix
			*/

			currentVertexTemp = i;
			currentTriTemp = i;

			/*
			currentVertexTemp = i;
			
			// convert Mesh Data -> World Space
			
			//rotateMeshFace(v1, v2, v3, mesh->pos, mesh->pitch, mesh->yaw); // TODO: CAN WE NOT DO THIS BEFORE SCALING AND POSITIONING? 
			
			// rotate mesh face - before scaling and translating so we do not need to re-position
			rotateV3(v1, mesh->pitch, mesh->yaw);
			rotateV3(v2, mesh->pitch, mesh->yaw);
			rotateV3(v3, mesh->pitch, mesh->yaw); // TODO: Test this is actually working.

			// scale vertices to mesh size
			v1 *= mesh->size;
			v2 *= mesh->size;
			v3 *= mesh->size;

			// translate mesh into mesh position
			v1 += entity->physics->position;
			v2 += entity->physics->position;
			v3 += entity->physics->position;
			
			// convert from World Space -> Camera Space
			
			// TODO: think of a way to transform coordinates to world coordinates?
			// the problem is that if we're moving constantly, we will constantly have to reconstruct,
			// meaning another loop through the vertices

			// apply view transformation
			viewTransform(v1);
			viewTransform(v2);
			viewTransform(v3);

			// apply camera rotation
			rotateV3(v1, camera->pitch, camera->yaw);
			rotateV3(v2, camera->pitch, camera->yaw);
			rotateV3(v3, camera->pitch, camera->yaw);
			*/

			Colour c1 = v1.c;
			Colour c2 = v2.c;
			Colour c3 = v3.c;

			v1 = v1 * model_view;
			v2 = v2 * model_view;
			v3 = v3 * model_view;

			v1.c = c1;
			v2.c = c2;
			v3.c = c3;

			// cull backfaces
			if (isFrontFacing(v1, v2, v3)) {
				// if the triangle is facing the camera, add it to the draw queue
				Triangle3D tri(v1, v2, v3);
				tri.colour = mesh->colours[i];
				triangles.push_back(tri);
			}
		}

		// frustum culling
		// determine if the mesh is fully inside the frustum
		int state = viewFrustum.isMeshVisible(triangles); // 0 = fully inside, 1 = fully outside, 2 = mesh intersects plane 
		
		if (state == 1) {
			// whole mesh is outside of the viewing frustum so ignore
			continue;
		}
		else if (state == 2) {
			// mesh is partly outside of the viewing frustum so cull
			// TODO: Its the clipped ones that break
			// TODO: I BELIEVE ITS BECAUSE OF A NEGATIVE DISTANCE????
			viewFrustum.clipTriangles(triangles);

		}

		// loop through clipped triangles
		for (int t = 0; t < triangles.size(); ++t) {
			// lighting
			Colour base_colour = triangles[t].colour;
			Colour colour_v1, colour_v2, colour_v3;

			bool fill = true;

			currentTriTemp = t;

			
			
			// only apply lighting to filled meshes
			if (fill && mesh->absorbsLight) {
				// apply point lighting
				getVertexColours(triangles[t].v1, triangles[t].v2, triangles[t].v3, base_colour, colour_v1, colour_v2, colour_v3);
			}
			else {
				colour_v1 = base_colour;
				colour_v2 = base_colour;
				colour_v3 = base_colour;
			}

			//colour_v2 = colour_v3;
			//colour_v1 = colour_v3;

			
			
			// project vertices to 2D - Camera Space -> Screen Space
			V2 pv1 = project(triangles[t].v1);
			V2 pv2 = project(triangles[t].v2);
			V2 pv3 = project(triangles[t].v3);


			// TODO: FRUSTUM CULLING ISN'T SETTING THE TRIANGLE.V.C FOR SOME REASON??????
			colour_v1 = triangles[t].v1.c;
			colour_v2 = triangles[t].v2.c;
			colour_v3 = triangles[t].v3.c;

			colour_v1.print();
			colour_v2.print();
			colour_v3.print();
			std::cout << std::endl;

			//colour_v1 *= pv1.w;
			//colour_v2 *= pv2.w;
			//colour_v3 *= pv3.w;

			// this is painfully slow.
			// clipping 2d triangles shouldn't be the solution as this would create more triangles to draw.
			drawTriangle(pv1, pv2, pv3, colour_v1, colour_v2, colour_v3); // very slow.
			

			//drawLine(pv1, pv3, COLOUR::WHITE);
			//drawLine(pv1, pv2, COLOUR::WHITE);
			//drawLine(pv2, pv3, COLOUR::WHITE);
		}
	}
	
	// draw crosshair
	// calculate the center of the page (using bit shift for faster divide by 2)
	int center_x = WN_WIDTH >> 1;
	int center_y = WN_HEIGHT >> 1;

	int thickness = 2;

	// draw rectangle
	drawRectangle(center_x - thickness, center_y - thickness, center_x + thickness, center_y + thickness, 0xFFFFFFFF);

	// testing
	/*
	int x, y;
	SDL_GetMouseState(&x, &y);
	V2 pv1(x, y);
	V2 pv2(200, 200);
	V2 pv3(450, 225);

	Colour colour_v1(1.0f, 0.0f, 0.0f);
	Colour colour_v2(0.0f, 1.0f, 0.0f);
	Colour colour_v3(0.0f, 0.0f, 1.0f);
	*/
	//drawTriangle(pv1, pv2, pv3, colour_v1, colour_v2, colour_v3, true);

}

void Renderer::display() {
	SDL_UpdateWindowSurface(window);
}

void Renderer::cleanup() {

	IMG_Quit();

	// cleanup all resources
	SDL_DestroyWindow(window);
}