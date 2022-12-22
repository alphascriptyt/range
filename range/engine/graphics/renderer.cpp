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

	// check for failures creating the rendersurface
	if (!renderSurface.init(window, WN_WIDTH, WN_HEIGHT, COLOUR::WHITE.toInt(), nearPlane)) {
		return false;
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

	// render the scanline
	for (; x1 <= x2; ++x1) {
		// set the pixel via index
		//renderSurface.setIndex(x1, colour.toInt(), z1); // TODO: I think this is actually the slow part!! look into this.
		renderSurface.setIndex(x1, ((int)(r) << 16) | ((int)(g) << 8) | ((int)(b)), z1); // TODO: I think this is actually the slow part!! look into this.
		
		//renderSurface.setIndex(x1, c + cs * x_percent, z1);
		// increment depth and colour
		z1 += z_increment;
		//colour += colour_step;

		r += r_step;
		g += g_step;
		b += b_step;
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

	// precalculate vars
	float y3_sub_y1 = (v3.y - v1.y);
	float y3_sub_y2 = (v3.y - v2.y);

	// calculate dx/dy for each triangle side
	float m1 = (v3.x - v1.x) / y3_sub_y1;
	float m2 = (v3.x - v2.x) / y3_sub_y2;

	// calculate dz/dy for each triangle side
	float mz1 = (v3.w - v1.w) / y3_sub_y1;
	float mz2 = (v3.w - v2.w) / y3_sub_y2;

	// set initial start and end x of line
	float start_x = v3.x;
	float end_x = v3.x;

	// set initial start and end depth of line
	float start_z = v3.w;
	float end_z = v3.w;

	// calculate colour gradients for each triangle side
	Colour mc1 = (colour_v3 - colour_v1) / y3_sub_y1;
	Colour mc2 = (colour_v3 - colour_v2) / y3_sub_y2;

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
	// sort vertices in ascending order
	if (v1.y > v2.y) { std::swap(v1, v2); std::swap(colour_v1, colour_v2); } // ensure v2 is bigger than v1
	if (v1.y > v3.y) { std::swap(v1, v3); std::swap(colour_v1, colour_v3); } // ensure v3 is bigger than v1
	if (v2.y > v3.y) { std::swap(v2, v3); std::swap(colour_v2, colour_v3); } // ensure v3 is bigger than v2

	// check if triangles already has a flat top/bottom 
	if (v1.y == v2.y) { drawFlatTopTriangle(v1, v2, v3, colour_v1, colour_v2, colour_v3); return; }
	if (v2.y == v3.y) { drawFlatBottomTriangle(v1, v2, v3, colour_v1, colour_v2, colour_v3); return; }

	// calculate v4's x (opposite side from v2)
	float x = v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x); // lerp for x

	// get v4's depth
	float mz = (v3.w - v1.w) / (v3.x - v1.x);	// get gradient of depths
	float w = v1.w + ((x - v1.x) * mz);			// linear interpolate to get v4's depth

	V2 v4(x, v2.y, w);

	// get v4's colour
	Colour mc = (colour_v3 - colour_v1) / (v3.x - v1.x);	// get gradient of depths
	Colour colour_v4 = colour_v1 + (mc * (x - v1.x));			// linear interpolate to get v4's depth

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
	// convert a 4D matrix to 2D pixel coordinates
	V3 projected = projectionMatrix * rotated;

	// scale to screen
	V2 pixel_coords;

	// avoid divide by 0 
	if (projected.w != 0) {
		// z component of V3 is copied to w
		// divide by w, to correct projection for perspective
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

void Renderer::drawLine3D(V3& v1, V3& direction, float length) {
	// calculate the end point
	V3 v2 = v1 + direction * length;

	// project the start and end coordinates to 2D
	V2 start = project(v1);
	V2 end = project(v2);

	if (start.x > end.x) { std::swap(start, end); }

	drawLine(start, end, COLOUR::LIME);
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
	// calculate the direction of the light to the vertex
	V3 light_direction = light_pos - v;
	float light_distance = light_direction.size(); 
	

	//drawLine3D(v, light_direction, light_distance);

	if (currentVertexTemp == 11 && currentTriTemp == 0 && currentTriVertexTemp == 0 && currentMeshTemp == 0) {
		light_direction.print();
		std::cout << light_distance << std::endl;
		// FIXME: Issue found, light_distance changes, should never change right?
		// NOTE: The distance works until a certain point?? Some underlying issue here.
		// This means that light_pos isn't changing at the same rate as v, which it should be...
		// this means they're not in the same space?

		// NOTE: size has something to do with it because disabling this changes it?
		// moving the mouse still changes the distance though, why???? are we rotating around a different
		// origin because the mesh has its position and size?

	}

	light_direction /= light_distance; // normalise using the pre-calculated size
	
	//drawLine3D(light_pos, light_direction, 3);

	// coordinates get flipped so we must invert the light direction
	//light_direction *= -1;

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
	ambient_part *= ambient_level;

	// calculate the lines between vertices
	V3 line1 = v2 - v1;
	V3 line2 = v3 - v1;

	// calculate the face normal
	V3 normal = vectorCrossProduct(line1, line2);
	normal.normalize();
	
	// loop through each lightsource
	for (int l = 0; l < LightSource::sources.size(); ++l) {
		if (l == 0) continue; // TEMP: TESTING

		// get the pointer to the lightsource
		LightSource* light = LightSource::sources[l];

		// only calculate if light enabled
		if (light->enabled) {
			// convert the light to camera space
			V3 light_pos = light->position;
			
			// transform light position into camera space

			// apply view transform
			viewTransform(light_pos);
			
			// apply camera rotation
			rotateV3(light_pos, camera->pitch, camera->yaw);

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
	colour_v1 = base_colour * (diffuse_part_v1 + ambient_part);
	colour_v2 = base_colour * (diffuse_part_v2 + ambient_part);
	colour_v3 = base_colour * (diffuse_part_v3 + ambient_part);

	if (colour_v1.r > 0.2 && currentVertexTemp == 11) {
		// FIXME: essentially colour_V1 should never change right?
		// because the only light is fixed?

		//std::cout << "Red v1 " << std::endl;
		//v1.print();
		//colour_v1.print();
	}
}

void Renderer::renderScene(Scene* scene) {
	// clear render surface
	//renderSurface.clear(0x87CEEB, farPlane);
	renderSurface.clear(0, farPlane);

	//M4 view = camera->makeViewMatrix().getInverse();
	M4 view = camera->makeViewMatrix();
	
	//view.print();
	
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

		currentMeshTemp = s;

		M4 model = entity->makeModelMatrix();
		M4 model_view = view * model;
		//model_view.print();
		
		// loop through each face in mesh
		for (int i = 0; i < mesh->faces.size(); ++i) {
			// get mesh face vertices
			V3 v1(mesh->vertices[mesh->faces[i][0]]);
			V3 v2(mesh->vertices[mesh->faces[i][1]]);
			V3 v3(mesh->vertices[mesh->faces[i][2]]);

			/*
			STEPS:
			Model Space -> World Space:		Model Matrix
			World Space -> Camera Space:		View Matrix
			View Space	-> Clip Space:		Projection Matrix


			M4 modelView = model * view; 
			
			*/

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

			v1 = model_view * v1;
			v2 = model_view * v2;
			v3 = model_view * v3;

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

			currentTriTemp = t;
			
			// only apply lighting to filled meshes
			if (fill && mesh->absorbsLight) {
				// apply point lighting
				getVertexColours(triangles[t].v1, triangles[t].v2, triangles[t].v3, base_colour, colour_v1, colour_v2, colour_v3);
			}

			// project vertices to 2D - Camera Space -> Screen Space
			V2 pv1 = project(triangles[t].v1);
			V2 pv2 = project(triangles[t].v2);
			V2 pv3 = project(triangles[t].v3);

			// this is painfully slow.
			// clipping 2d triangles shouldn't be the solution as this would create more triangles to draw.
			drawTriangle(pv1, pv2, pv3, colour_v1, colour_v2, colour_v3); // very slow.

			drawLine(pv1, pv3, COLOUR::WHITE);
			drawLine(pv1, pv2, COLOUR::WHITE);
			drawLine(pv2, pv3, COLOUR::WHITE);
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
	// cleanup all resources
	SDL_DestroyWindow(window);
}