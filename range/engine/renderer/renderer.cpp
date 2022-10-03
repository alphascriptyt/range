#include "SDL.h"
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
	// pre calculate the projection matrix once at the start as it shouldn't change
	float aspect_ratio = WN_WIDTH / (float)WN_HEIGHT;

	projectionMatrix.setRow(0, 1 / halfTanFOV, 0, 0, 0);
	projectionMatrix.setRow(1, 0, aspect_ratio / halfTanFOV, 0, 0);
	projectionMatrix.setRow(2, 0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), -1);
	projectionMatrix.setRow(3, 0, 0, (2 * nearPlane * farPlane) / (nearPlane - farPlane), 0);
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

	// check for failures creating the buffer
	if (!initBuffers()) {
		return false;
	}
	
	// create the viewpoint lightsource (has to be created first so we can access it easily)
	Scene::scenes[0]->createLightSource("viewpoint", camera->physics.position, COLOUR::WHITE, 10);

	return true;
}

bool Renderer::initBuffers() {
	// try get a pointer to the SDL window surface
	windowSurface = SDL_GetWindowSurface(window);
	
	// check for errors
	if (!windowSurface) {
		printf("Failed to GetWindowSurface: %s\n", SDL_GetError());
		return false;
	}
	
	// get access to the pixel buffer 
	pixelBuffer = (uint32_t*)windowSurface->pixels;

	// find the buffer's pitch (number of bytes in one row of buffer)
	bufferPitch = windowSurface->pitch;

	// calculate the length of the buffer
	bufferLength = windowSurface->w * windowSurface->h;
	
	// initialize depth buffer with z-far
	for (int i = 0; i < bufferLength; ++i) {
		depthBuffer.push_back(farPlane);
	}

	// create projection matrix
	createProjectionMatrix();

	return true;
}

// depth testing
bool Renderer::testAndSetDepth(int pos, float z) {
	// overwrite pixel in buffer if closer to camera
	if (depthBuffer[pos] > z) {
		depthBuffer[pos] = z;

		return true;	// draw pixel
	}
	return false;		// don't draw pixel
}

// 2D primitive drawing 
void Renderer::drawScanLine(int x1, int x2, int y, Colour colour_v1, Colour colour_v2, float z1, float z2, bool fill) {
	// if y is off screen or y is below screen, ignore
	if (y > (WN_HEIGHT - 1) || y < 0) { return; }

	// ensure coords go left to right
	if (x1 > x2) { std::swap(x1, x2); std::swap(colour_v1, colour_v2); }

	// ignore line that starts past right side
	if (x1 > WN_WIDTH - 1) { return; }

	// ignore point past left side
	if (x2 < 0) { return; }

	// clip point to left side
	if (x1 < 0) {
		colour_v1 = colour_v1 + ((colour_v2 - colour_v1) * ((-x1) / (float)(x2 - x1)));
		x1 = 0;
	}

	// clip point to right side
	if (x2 > WN_WIDTH - 1) {
		colour_v2 = colour_v2 - ((colour_v2 - colour_v1) * ((x2 - WN_WIDTH) / (float)(x2 - x1)));
		x2 = WN_WIDTH - 1;
	}

	// scale x in terms of y
	x1 += WN_WIDTH * y;
	x2 += WN_WIDTH * y;

	// fill line in array
	if (fill) {
		// calculate depth increment
		float z_increment = (z2 - z1) / ((float)x2 - x1);

		// calculate colour increment
		Colour colour_step = (colour_v2 - colour_v1) / (float)(x2 - x1);
		Colour colour = colour_v1;

		for (; x1 < x2; ++x1) {
			// perform depth testing for pixel
			if (testAndSetDepth(x1, z1)) {
				pixelBuffer[x1] = colour.toInt();
			}

			// increment depth and colour
			z1 += z_increment;
			colour += colour_step;
		}
	}
	// just draw the first and last pixel
	else {
		// perform depth testing for x1 and x2
		if (testAndSetDepth(x1, z1)) {
			pixelBuffer[x1] = colour_v1.toInt();;
		}

		if (testAndSetDepth(x2, z2)) {
			pixelBuffer[x2] = colour_v2.toInt();
		}
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
			pixelBuffer[x] = colour;
		}
	}

}

void Renderer::drawFlatBottomTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3, bool fill) {
	// here we draw each scanline from the highest vertex v1
	if (v2.x > v3.x) { std::swap(v2, v3); std::swap(colour_v2, colour_v3); } // ensure v3 is bigger than v2

	// calculate dx/dy for each triangle side
	float m1 = (v2.x - v1.x) / (v2.y - v1.y);
	float m2 = (v3.x - v1.x) / (v3.y - v1.y);

	// calculate dz/dy for each triangle side
	float mz1 = (v2.w - v1.w) / (v2.y - v1.y);
	float mz2 = (v3.w - v1.w) / (v3.y - v1.y);

	// set initial start and end x of line
	float start_x = v1.x;
	float end_x = v1.x;

	// set initial start and end depth of line
	float start_z = v1.w;
	float end_z = v1.w;

	// calculate colour gradients for each triangle side
	Colour mc1 = (colour_v2 - colour_v1) / (v2.y - v1.y);
	Colour mc2 = (colour_v3 - colour_v1) / (v3.y - v1.y);

	// calculate colours for start and end of lines
	Colour start_colour = colour_v1;
	Colour end_colour = colour_v1;

	// step through y to calculate start and end of scanline
	for (int y = v1.y; y <= v2.y; ++y) {
		// draw the scanline
		//drawScanLine(start_x, end_x, y, colour, start_z, end_z, fill);
		drawScanLine(start_x, end_x, y, start_colour, end_colour, start_z, end_z, fill);

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

void Renderer::drawFlatTopTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3, bool fill) {
	if (v1.x > v2.x) { std::swap(v1, v2); std::swap(colour_v1, colour_v2); } // ensure v3 is bigger than v2

	// calculate dx/dy for each triangle side
	float m1 = (v3.x - v1.x) / (v3.y - v1.y);
	float m2 = (v3.x - v2.x) / (v3.y - v2.y);

	// calculate dz/dy for each triangle side
	float mz1 = (v3.w - v1.w) / (v3.y - v1.y);
	float mz2 = (v3.w - v2.w) / (v3.y - v2.y);

	// set initial start and end x of line
	float start_x = v3.x;
	float end_x = v3.x;

	// set initial start and end depth of line
	float start_z = v3.w;
	float end_z = v3.w;

	// calculate colour gradients for each triangle side
	Colour mc1 = (colour_v3 - colour_v1) / (v3.y - v1.y);
	Colour mc2 = (colour_v3 - colour_v2) / (v3.y - v2.y);

	// calculate colours for start and end of lines
	Colour start_colour = colour_v3;
	Colour end_colour = colour_v3;

	// step through y to calculate start and end of scanline
	for (int y = v3.y; y > v1.y; y--) {
		// draw the scanline
		//drawScanLine(start_x, end_x, y, colour, start_z, end_z, fill);
		drawScanLine(start_x, end_x, y, start_colour, end_colour, start_z, end_z, fill);

		// for each decrement of y, step through x by dx/dy
		start_x -= m1;
		end_x -= m2;

		start_z -= mz1;
		end_z -= mz2;

		// step through z by dz/dy
		start_z += mz1;
		end_z += mz2;

		// step through colour by dc/dy
		start_colour -= mc1;
		end_colour -= mc2;
	}
}

void Renderer::drawTriangle(V2& v1, V2& v2, V2& v3, Colour& colour_v1, Colour& colour_v2, Colour& colour_v3, bool fill) {
	// sort vertices in ascending order
	if (v1.y > v2.y) { std::swap(v1, v2); std::swap(colour_v1, colour_v2); } // ensure v2 is bigger than v1
	if (v1.y > v3.y) { std::swap(v1, v3); std::swap(colour_v1, colour_v3); } // ensure v3 is bigger than v1
	if (v2.y > v3.y) { std::swap(v2, v3); std::swap(colour_v2, colour_v3); } // ensure v3 is bigger than v2
	
	// check if triangles already has a flat top/bottom 
	if (v1.y == v2.y) { drawFlatTopTriangle(v1, v2, v3, colour_v1, colour_v2, colour_v3, fill); return; }
	if (v2.y == v3.y) { drawFlatBottomTriangle(v1, v2, v3, colour_v1, colour_v2, colour_v3, fill); return; }

	// calculate v4's x (opposite side from v2)
	float x = v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x); // lerp for x

	// get v4's depth
	float mz = (v3.w - v1.w) / (v3.x - v1.x);	// get gradient of depths
	float w = v1.w + ((x - v1.x) * mz);			// linear interpolate to get v4's depth

	V2 v4(x, v2.y, w);

	// get v4's colour
	Colour mc = (colour_v3 - colour_v1) / (v3.x - v1.x);	// get gradient of depths
	Colour colour_v4 = colour_v1 + (mc * (x - v1.x));			// linear interpolate to get v4's depth


	//Colour colour_v4 = colour_v1 + ((colour_v3 - colour_v1) * ((v3.y - v1.y) / v2.y)); // lerp for colour // TODO: THIS APPEARS TO BE WRONG.
	
	//drawFlatBottomTriangle(v1, v2, v4, colour_v1, colour_v2, colour_v3, fill);
	//drawFlatTopTriangle(v2, v4, v3, colour_v1, colour_v2, colour_v3, fill);

	// draw split triangles
	drawFlatBottomTriangle(v1, v2, v4, colour_v1, colour_v2, colour_v4, fill);
	drawFlatTopTriangle(v2, v4, v3, colour_v2, colour_v4, colour_v3, fill);
}

// text rendering
void Renderer::renderText(char* text, Colour& colour, int x, int y) {
	// create a surface from the text
	SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, colour.toSDL());

	// set the text surface rect position
	text_surface->clip_rect.x = x;
	text_surface->clip_rect.y = y;

	// blit the text surface to the window surface
	SDL_BlitSurface(text_surface, NULL, windowSurface, &text_surface->clip_rect);
	
	// free the surface
	if (text_surface) { SDL_FreeSurface(text_surface); }
}

// rendering methods
void Renderer::viewTransform(V3& v) {
	// translate scene with respect to camera
	v.x -= camera->physics.position.x;
	v.y -= camera->physics.position.y;
	v.z -= camera->physics.position.z;
}

bool Renderer::backfaceCull(V3& v1, V3& v2, V3& v3) {
	// algorithm to check whether a face should be drawn
	// or if it is on the backside of the shape and therefore
	// cannot be seen
	V3 vs1 = v2 - v1;
	V3 vs2 = v3 - v1;
	V3 n = vectorCrossProduct(vs1, vs2);

	if (vectorDotProduct(v1, n) <= 0) {
		return true; // TODO: THIS IS A BIT MISLEADING, MAYBE CHANGE
	}
	return false;
}

V2 Renderer::project(V3& rotated) {
	// convert a 4D matrix to 2D pixel coordinates
	// the projection matrix is a standard matrix
	// taking near and far plane members which determine the bounds of whether
	// something should be rendered

	V3 projected = multiplyBy4x1(projectionMatrix, rotated);

	// scale to screen
	V2 pixel_coords;

	// avoid divide by 0 
	if (projected.w != 0) {
		// z component of V3 is copied to w
		// divide by w (z), to correct projection for perspective
		// so shape gets smaller as it gets further away
		pixel_coords.x = projected.x / projected.w;
		pixel_coords.y = projected.y / projected.w;
	}

	// center in screen
	pixel_coords.x += 1;
	pixel_coords.y += 1;

	pixel_coords.x *= 0.5 * WN_WIDTH;
	pixel_coords.y *= 0.5 * WN_HEIGHT;

	pixel_coords.w = 1.0f / projected.z;

	return pixel_coords;
}

void Renderer::setBackgroundColour(int colour) {
	std::fill(pixelBuffer, pixelBuffer + bufferLength, colour);
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

void Renderer::drawLine3D(V3& v1, V3& direction, float length) {
	// calculate the gap between the start and end of the line
	V3 gap = direction;

	gap.x *= length;
	gap.y *= length;
	gap.z *= length;

	V3 v2 = v1 + gap;

	// calculate the start and end coordinates in 2D
	V2 start = project(v1);
	V2 end = project(v2);

	if (start.x > end.x) { std::swap(start, end); }

	// linear interpolate for each pixel in the line
	int dx, dy, p, y;

	dx = end.x - start.x;
	dy = end.y - start.y;

	y = start.y;

	p = 2 * dy - dx;

	// standard line drawing algorithm
	for (int x = start.x; x < end.x; ++x) {
		bool visible = true;

		// if y is off screen or y is below screen, ignore
		if (y > (WN_HEIGHT - 1) || y < 0) { visible = false; }

		// ignore line that starts past right side
		if (x > WN_WIDTH - 1) { visible = false; }

		// ignore point past left side
		if (x < 0) { visible = false; }

		if (p >= 0) {
			if (visible) {
				pixelBuffer[x + (y * WN_WIDTH)] = COLOUR::LIME.toInt();
				depthBuffer[x + (y * WN_WIDTH)] = -1;
			}

			y = y + 1;
			p = p + 2 * dy - 2 * dx;
		}
		else {
			if (visible) {
				pixelBuffer[x + (y * WN_WIDTH)] = COLOUR::LIME.toInt();
				depthBuffer[x + (y * WN_WIDTH)] = -1;
			}

			p = p + 2 * dy;
		}
	}
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
	ambient_part.r *= ambient_level;
	ambient_part.g *= ambient_level;
	ambient_part.b *= ambient_level;

	// loop through each lightsource
	for (int l = 0; l < LightSource::sources.size(); ++l) {
		// get the pointer to the lightsource
		LightSource* light = LightSource::sources[l];

		// only calculate if light enabled
		if (light->enabled) {
			// calculate the diffuse factor for the point light with attenuation
			float dp = applyPointLighting(v1, v2, v3, *light);

			// add to the colour
			diffuse_part.r += dp * light->colour.r;
			diffuse_part.g += dp * light->colour.g;
			diffuse_part.b += dp * light->colour.b;
		}
	}

	// calculate the resultant colour off the face
	Colour face_colour(0, 0, 0);

	face_colour.r = (diffuse_part.r + ambient_part.r) * base_colour.r;
	face_colour.g = (diffuse_part.g + ambient_part.g) * base_colour.g;
	face_colour.b = (diffuse_part.b + ambient_part.b) * base_colour.b;
	
	// set the colour of the mesh
	base_colour = face_colour;
}

float Renderer::calculateDiffusePart(V3& v, V3& n, V3& light_pos, float a, float b) {
	// TODO: 90% sure something is going wrong here, the error comes from looking away so is the camera direction messing us up?


	// calculate the direction of the light to the vertex
	V3 light_direction = light_pos - v;
	float light_distance = light_direction.size();
	light_direction.normalize();

	// coordinates get flipped so we must invert the light direction
	//light_direction *= -1;

	// calculate how much the vertex is lit
	float diffuse_factor = vectorDotProduct(light_direction, n); // diffuse factor gets bigger the further away???
	diffuse_factor = std::max(0.0f, diffuse_factor);
	float dp = 0.0f;

	// determine if the vertex is lit by the lightsource
	
	// attenuation is correct
	float attenuation = 1.0f / (1.0f + (a * light_distance) + (b * light_distance * light_distance));
	dp = abs(diffuse_factor * attenuation);

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
	ambient_part.r *= ambient_level;
	ambient_part.g *= ambient_level;
	ambient_part.b *= ambient_level;

	// calculate the lines between vertices
	V3 line1 = v2 - v1;
	V3 line2 = v3 - v1;

	// calculate the face normal
	V3 normal = vectorCrossProduct(line1, line2);
	normal.normalize();

	// loop through each lightsource
	for (int l = 0; l < LightSource::sources.size(); ++l) {
		// get the pointer to the lightsource
		LightSource* light = LightSource::sources[l];

		// only calculate if light enabled
		if (light->enabled) {
			// convert the light to camera space
			V3 L = light->position;

			// TODO: I BELIEVE THE ERROR IS SOMEWHERE HERE? LOOKING AWAY ESSENTIALLY BREAKS IT?

			// transform light position into camera space - is this necessary?

			// apply view transform
			viewTransform(L);
			
			// apply camera rotation
			rotateV3(L, camera->pitch, camera->yaw);

			// calculate the light attenuation parameters
			float a = 0.1f / light->strength;
			float b = 0.01f / light->strength;

			float dp = calculateDiffusePart(v1, normal, L, a, b);

			// add to the colour
			diffuse_part_v1.r += dp * light->colour.r;
			diffuse_part_v1.g += dp * light->colour.g;
			diffuse_part_v1.b += dp * light->colour.b;

			dp = calculateDiffusePart(v2, normal, L, a, b);

			// add to the colour
			diffuse_part_v2.r += dp * light->colour.r;
			diffuse_part_v2.g += dp * light->colour.g;
			diffuse_part_v2.b += dp * light->colour.b;

			dp = calculateDiffusePart(v3, normal, L, a, b);

			// add to the colour
			diffuse_part_v3.r += dp * light->colour.r;
			diffuse_part_v3.g += dp * light->colour.g;
			diffuse_part_v3.b += dp * light->colour.b;
		}
	}

	// calculate the resultant colour off the face
	colour_v1 = base_colour * (diffuse_part_v1 + ambient_part);
	colour_v2 = base_colour * (diffuse_part_v2 + ambient_part);
	colour_v3 = base_colour * (diffuse_part_v3 + ambient_part);
	


}

void Renderer::renderScene(Scene* scene) {
	// clear screen
	setBackgroundColour(COLOUR::WHITE.toInt());

	// initialize depth buffer with z-far (maximum z distance)
	std::fill(depthBuffer.begin(), depthBuffer.end(), farPlane); // TODO: is farPlane working right?

	// actually render the scene
	// loop through each mesh
	//for (int m = 0; m < Mesh::meshes.size(); ++m) {
	for (int s = 0; s < scene->entities.size(); ++s) {
		//break;
		//Mesh* mesh = Mesh::meshes[m]; // get the pointer to the mesh
		Entity* entity = scene->entities[s];
		Mesh* mesh = entity->mesh; // get the pointer to the mesh
		// store all of the mesh triangles
		std::vector<Triangle3D> triangles;

		// loop through each face in mesh
		for (int i = 0; i < mesh->faces.size(); ++i) {
			// get mesh face vertices
			V3 v1(mesh->vertices[mesh->faces[i][0]]);
			V3 v2(mesh->vertices[mesh->faces[i][1]]);
			V3 v3(mesh->vertices[mesh->faces[i][2]]);
			
			// convert from World Space -> Camera Space
			
			//rotateMeshFace(v1, v2, v3, mesh->pos, mesh->pitch, mesh->yaw); // TODO: CAN WE NOT DO THIS BEFORE SCALING AND POSITIONING? 
			
			// rotate mesh face - before scaling and translating so we do not need to re-position
			rotateV3(v1, mesh->pitch, mesh->yaw);
			rotateV3(v2, mesh->pitch, mesh->yaw);
			rotateV3(v3, mesh->pitch, mesh->yaw);

			// scale vertices to mesh size
			v1 *= mesh->size;
			v2 *= mesh->size;
			v3 *= mesh->size;

			// translate mesh into mesh position
			v1 += entity->physics->position;
			v2 += entity->physics->position;
			v3 += entity->physics->position;

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

			// cull backfaces
			if (backfaceCull(v1, v2, v3)) {
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
			// whole mesh is outside of the viewing frustum
			continue;
		}
		else if (state == 2) {
			// mesh is partly outside of the viewing frustum so cull
			viewFrustum.clipTriangles(triangles);
		}

		// loop through clipped triangles
		for (int t = 0; t < triangles.size(); ++t) {
			// lighting
			Colour base_colour = triangles[t].colour;
			Colour colour_v1, colour_v2, colour_v3;

			bool fill = true;
			
			// only apply lighting to filled meshes
			if (fill && mesh->absorbsLight) {
				// apply point lighting
				//applyLighting(triangles[t].v1, triangles[t].v2, triangles[t].v3, base_colour);

				
				getVertexColours(triangles[t].v1, triangles[t].v2, triangles[t].v3, base_colour, colour_v1, colour_v2, colour_v3);
			}

			// TODO: WE SHOULD BE LIGHTING BEFORE WE CLIP. THEN WE DON'T NEED TO CONVERT EVERYTHING BACK?? but that wouldn't work
			// need to think about textures etc :/
			// i don't  think im oging to be  using textures, so what i need to do is lerp for each colour of each
			// vertex of the broken down triangles from the original triangles vertices. hopefully.....

			// project vertices to 2D - Camera Space -> Screen Space
			V2 pv1 = project(triangles[t].v1);
			V2 pv2 = project(triangles[t].v2);
			V2 pv3 = project(triangles[t].v3);

			// this is painfully slow.
			// clipping 2d triangles shouldn't be the solution as this would create more triangles to draw.
			//drawTriangle(pv1, pv2, pv3, colour.toInt(), fill); // very slow.
			drawTriangle(pv1, pv2, pv3, colour_v1, colour_v2, colour_v3, fill);
		}
	}
	
	// draw crosshair
	// calculate the center of the page (using bit shift for faster divide by 2)
	int center_x = WN_WIDTH >> 1;
	int center_y = WN_HEIGHT >> 1;

	int thickness = 2;

	// draw rectangle
	drawRectangle(center_x - thickness, center_y - thickness, center_x + thickness, center_y + thickness, COLOUR::WHITE.toInt());

	/*
	for (int i = 0; i < 50; ++i) {
		Colour yellow(1,1,0);
		Colour red(1,0,0);
		drawScanLine(700, 1500, 500+i, yellow, red);
		drawScanLine(700, 960, 600+i, yellow, red);
		// TODO: problem found. - problem sorted?
	}
	*/
	
	/*
	int x, y;
	SDL_GetMouseState(&x, &y);
	V2 v1(300, 300);
	V2 v2(600, 100);
	V2 v3(x, y);

	Colour red(1, 0, 0);
	Colour blue(0, 0, 1);
	Colour green(0, 1, 0);

	drawTriangle(v1, v2, v3, red, green, blue);
	*/

	// TODO:
	// ISSUE: Looks like lerping code is fine now, however, some vertices seem to turn black?
	// this only happens when looking certain directions so could this actually be the lighting now?
	// I believe the lerping code is all fixed. Well done :)
	// occasionally get like a zebra pattern so everything can't be working right

	// when all vertices are the same colour it works fine, so this means the lerping code is wrong or the lighting code is wrong?

}

void Renderer::cleanup() {
	// cleanup all resources
	SDL_DestroyWindow(window);
}