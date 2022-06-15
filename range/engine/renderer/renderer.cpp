#include "SDL.h"
#include "mesh.h"
#include "renderer.h"
#include "mat4D.h"
#include "vectormaths.h"
#include "triangle.h"
#include "lightsource.h"

#include <cmath>
#include <iostream>
#include <limits>

// setup
Renderer::Renderer() {

}

Renderer::Renderer(int w, int h) {
	WN_WIDTH = w;
	WN_HEIGHT = h;
}

void Renderer::createProjectionMatrix() {
	// create the projection matrix once at the start as it shouldn't change
	float aspect_ratio = WN_WIDTH / (float)WN_HEIGHT;

	projectionMatrix.setRow(0, 1 / halfTanFOV, 0, 0, 0);
	projectionMatrix.setRow(1, 0, aspect_ratio / halfTanFOV, 0, 0);
	projectionMatrix.setRow(2, 0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), -1);
	projectionMatrix.setRow(3, 0, 0, (2 * nearPlane * farPlane) / (nearPlane - farPlane), 0);
}

bool Renderer::init() {
	// create window
	window = SDL_CreateWindow("Test", 50, 50, WN_WIDTH, WN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!window) {
		printf("Failed to create window - %s\n", SDL_GetError());
		return false;
	}

	// create renderer
	SDL_RENDERER = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!SDL_RENDERER) {
		printf("Failed to create renderer - %s\n", SDL_GetError());
		return false;
	}

	// check for failures creating the buffer
	if (!initBuffers()) {
		return false;
	}

	// create the viewpoint lightsource (has to be created first so we can access it easily)
	LightSource* viewpoint = new LightSource(camera.position, COLOUR::WHITE, 10); // create on heap so GC doesn't delete it

	return true;
}
bool Renderer::initBuffers() {
	// create a texture for rendering pixel buffer as RGBA
	bufferTexture = SDL_CreateTexture(SDL_RENDERER,
		SDL_PIXELFORMAT_ABGR32,			// RGBA 
		SDL_TEXTUREACCESS_STREAMING,	// allow for texture to be locked and unlocked for streaming
		WN_WIDTH,
		WN_HEIGHT);

	if (bufferTexture == nullptr)
	{
		printf("Could not create a buffer texture: %s\n", SDL_GetError());
		return false;
	}

	// lock memory so we can write to it (needed for first frame)
	SDL_LockTexture(bufferTexture, NULL, (void**)&pixelBuffer, &bufferPitch);

	// calculate number of bytes in one row of buffer
	bufferLength = WN_WIDTH * WN_HEIGHT;

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

		return true; // pixel should be drawn
	}
	return false; // pixel shouldn't be drawn
}

// 2D primitive drawing 
void Renderer::drawScanLine(int x1, int x2, int y, int colour, float z1, float z2, bool fill) {
	// if y is off screen or y is below screen, ignore
	if (y > (WN_HEIGHT - 1) || y < 0) { return; }

	// ensure coords go left to right
	if (x1 > x2) { std::swap(x1, x2); }

	// ignore line that starts past right side
	if (x1 > WN_WIDTH - 1) { return; }

	// ignore point past left side
	if (x2 < 0) { return; }

	// clip point to left side
	if (x1 < 0) {
		x1 = 0;
	}

	// clip point to right side
	if (x2 > WN_WIDTH - 1) {
		x2 = WN_WIDTH - 1;
	}

	// scale x in terms of y
	x1 += WN_WIDTH * y;
	x2 += WN_WIDTH * y;

	// fill line in array
	if (fill) {
		float z_increment = (z2 - z1) / ((float)x2 - x1);
		for (; x1 < x2; ++x1) {
			// increment z1
			z1 += z_increment;

			// perform depth testing for pixel
			if (testAndSetDepth(x1, z1)) {
				pixelBuffer[x1] = colour;
			}
		}
	}
	// just draw the first and last pixel
	else {
		// perform depth testing for x1 and x2
		if (testAndSetDepth(x1, z1)) {
			pixelBuffer[x1] = colour;
		}

		if (testAndSetDepth(x2, z2)) {
			pixelBuffer[x2] = colour;
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

void Renderer::drawFlatBottomTriangle(V2& v1, V2& v2, V2& v3, int colour, bool fill) {
	// calculate dx/dy for each triangle side
	float m1 = (v2.x - v1.x) / (v2.y - v1.y);
	float m2 = (v3.x - v1.x) / (v3.y - v1.y);

	// calculate dz/dy for each triangle side
	float mz1 = (v2.w - v1.w) / (v2.y - v1.y);
	float mz2 = (v3.w - v1.w) / (v3.y - v1.y);

	// calculate initial x coordinate
	float start_x = v1.x;
	float end_x = v1.x;

	float start_z = v1.w;
	float end_z = v1.w;

	// step through y to calculate start and end of scanline
	for (int y = v1.y; y <= v2.y; ++y) {
		// draw the scanline
		drawScanLine(start_x, end_x, y, colour, start_z, end_z, fill);

		// for each increment of y, step through x by dx/dy
		start_x += m1;
		end_x += m2;

		// step through z by dz/dy
		start_z += mz1;
		end_z += mz2;
	}
}

void Renderer::drawFlatTopTriangle(V2& v1, V2& v2, V2& v3, int colour, bool fill) {
	// calculate dx/dy for each triangle side
	float m1 = (v3.x - v1.x) / (v3.y - v1.y);
	float m2 = (v3.x - v2.x) / (v3.y - v2.y);

	// calculate dz/dy for each triangle side
	float mz1 = (v3.w - v1.w) / (v3.y - v1.y);
	float mz2 = (v3.w - v2.w) / (v3.y - v2.y);

	// calculate initial x coordinate
	float start_x = v3.x;
	float end_x = v3.x;

	float start_z = v3.w;
	float end_z = v3.w;

	// step through y to calculate start and end of scanline
	for (int y = v3.y; y > v1.y; y--) {
		// draw the scanline
		drawScanLine(start_x, end_x, y, colour, start_z, end_z, fill);

		// for each decrement of y, step through x by dx/dy
		start_x -= m1;
		end_x -= m2;

		start_z -= mz1;
		end_z -= mz2;
	}
}

void Renderer::drawTriangle(V2& v1, V2& v2, V2& v3, int colour, bool fill) {
	// sort vertices in ascending order
	if (v1.y > v2.y) { std::swap(v1, v2); } // ensure v2 is bigger than v1
	if (v1.y > v3.y) { std::swap(v1, v3); } // ensure v3 is bigger than v1
	if (v2.y > v3.y) { std::swap(v2, v3); } // ensure v3 is bigger than v2

	// check if triangle already has a flat top/bottom 
	if (v1.y == v2.y) { drawFlatTopTriangle(v1, v2, v3, colour, fill); return; }
	if (v2.y == v3.y) { drawFlatBottomTriangle(v1, v2, v3, colour, fill); return; }

	// calculate v4's x (opposite side from v2)
	float x = v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x); // lerp for x

	// get v4's depth
	float mz = (v3.w - v1.w) / (v3.x - v1.x);	// get gradient of depths
	float w = v1.w + ((x - v1.x) * mz);		// linear interpolate to get v4's depth

	V2 v4(x, v2.y, w);

	// draw split triangles
	drawFlatBottomTriangle(v1, v2, v4, colour, fill);
	drawFlatTopTriangle(v2, v4, v3, colour, fill);
}

// 3D methods
void Renderer::viewTransform(V3& v) {
	// translate scene with respect to camera
	v.x -= camera.position.x;
	v.y -= camera.position.y;
	v.z -= camera.position.z;
}

bool Renderer::backfaceCull(V3& v1, V3& v2, V3& v3) {
	// algorithm to check whether a face should be drawn
	// or if it is on the backside of the shape and therefore
	// cannot be seen
	V3 vs1 = vectorSub(v2, v1);
	V3 vs2 = vectorSub(v3, v1);
	V3 n = vectorCrossProduct(vs1, vs2);

	if (vectorDotProduct(v1, n) <= 0) {
		return true; // TODO: THIS IS A BIT MISLEADING, MAYBE CHANGE
	}

	return false;
}

float Renderer::findSignedDistance(V3& v, V3& plane_normal, V3& plane_point) {
	// method for finding the signed distance 
	// between a plane and a vertex

	// find the vector between the points
	V3 between = vectorSub(v, plane_point);

	// return the signed distance that the between vector goes
	// in the direction of the plane's normal
	return vectorDotProduct(plane_normal, between);
}

void Renderer::clipTriangle(V3& v1, V3& v2, V3& v3, std::vector<Triangle>& clipped) {
	// only clip the triangle to the near plane as its the only necessary one

	// define the near plane
	V3 near_plane_point = V3(0, 0, nearPlane);
	V3 near_plane_normal = V3(0, 0, 1);

	// store the points inside and outside of the near plane
	int inside_points_count = 0;
	int outside_points_count = 0;
	V3 inside_points[3];
	V3 outside_points[3];

	// calculate the distance away from the near plane
	float d1 = findSignedDistance(v1, near_plane_normal, near_plane_point);
	float d2 = findSignedDistance(v2, near_plane_normal, near_plane_point);
	float d3 = findSignedDistance(v3, near_plane_normal, near_plane_point);

	// determine which points are inside or outside of the plane
	if (d1 >= 0) {
		inside_points[inside_points_count] = v1;
		inside_points_count++;
	}
	else {
		outside_points[outside_points_count] = v1;
		outside_points_count++;
	}
	if (d2 >= 0) {
		inside_points[inside_points_count] = v2;
		inside_points_count++;
	}
	else {
		outside_points[outside_points_count] = v2;
		outside_points_count++;
	}
	if (d3 >= 0) {
		inside_points[inside_points_count] = v3;
		inside_points_count++;
	}
	else {
		outside_points[outside_points_count] = v3;
		outside_points_count++;
	}

	// clip vertices
	if (inside_points_count == 0) {
		// if all points are outside of the plane we ignore triangle
		return;
	}
	if (inside_points_count == 3) {
		// if all points are inside of the plane we can just
		// return the original triangle
		//Triangle* tri = new Triangle(v1, v2, v3);
		Triangle tri = Triangle(v1, v2, v3);
		clipped.push_back(tri);
		return;
	}
	if (inside_points_count == 1 && outside_points_count == 2) {
		// if only one vertex is inside the triangle
		// a new triangle will be formed using the plane edge

		V3 nv1 = vectorIntersectPlane(inside_points[0], outside_points[0], near_plane_normal, near_plane_point);
		V3 nv2 = vectorIntersectPlane(inside_points[0], outside_points[1], near_plane_normal, near_plane_point);

		Triangle tri = Triangle(inside_points[0], nv1, nv2);
		clipped.push_back(tri);
		return;
	}

	if (inside_points_count == 2 && outside_points_count == 1) {
		// if only two vertices are inside the plane
		// a quad will be formed and therefore two
		// new triangles will be formed
		V3 nv1 = vectorIntersectPlane(inside_points[0], outside_points[0], near_plane_normal, near_plane_point);
		Triangle tri1 = Triangle(inside_points[0], inside_points[1], nv1);
		clipped.push_back(tri1);

		V3 nv2 = vectorIntersectPlane(inside_points[1], outside_points[0], near_plane_normal, near_plane_point);
		Triangle tri2 = Triangle(inside_points[1], nv1, nv2);
		clipped.push_back(tri2);

		return;
	}
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

// rendering functions
void Renderer::setBackgroundColour(int colour) {
	std::fill(pixelBuffer, pixelBuffer + bufferLength, colour);
}

void Renderer::renderBuffer() {
	// render the buffer
	
	// lock memory so we can write to it
	SDL_LockTexture(bufferTexture, NULL, (void**)&pixelBuffer, &bufferPitch);

	// align in VRAM
	bufferPitch /= sizeof(uint32_t);

	// tell the GPU that we are done editing the texture
	SDL_UnlockTexture(bufferTexture);

	// copy our texture in VRAM to the display framebuffer in VRAM;
	SDL_RenderCopy(SDL_RENDERER, bufferTexture, NULL, NULL);

	// present renderer contents to display
	SDL_RenderPresent(SDL_RENDERER);

	// re-lock texture so we can edit buffer
	SDL_LockTexture(bufferTexture, NULL, (void**)&pixelBuffer, &bufferPitch);
}

void Renderer::rotateMeshFace(V3& v1, V3& v2, V3& v3, V3& pos, float pitch, float yaw) {
	// translate vertices to origin for rotation
	V3 tv1 = vectorSub(v1, pos);
	V3 tv2 = vectorSub(v2, pos);
	V3 tv3 = vectorSub(v3, pos);

	// rotate vertices
	rotateV3(tv1, pitch, yaw);
	rotateV3(tv2, pitch, yaw);
	rotateV3(tv3, pitch, yaw);

	// translate vertices back to original position
	v1 = vectorAdd(pos, tv1);
	v2 = vectorAdd(pos, tv2);
	v3 = vectorAdd(pos, tv3);
}

void Renderer::drawLine3D(V3& v1, V3& direction, float length) {
	// calculate the gap between the start and end of the line
	V3 gap = direction;

	gap.x *= length;
	gap.y *= length;
	gap.z *= length;

	V3 v2 = vectorAdd(v1, gap);

	// calculate the start and end coordinates in 2D
	V2 start = project(v1);
	V2 end = project(v2);

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
		if (y > (WN_HEIGHT - 1) || y < 1) { visible = false; }

		// ignore line that starts past right side
		if (x > (WN_WIDTH)) { visible = false; }

		// ignore point past left side
		if (x < 0) { visible = false; }

		if (p >= 0) {
			if (visible) {
				pixelBuffer[x + (y * WN_WIDTH)] = 0xFF000000;
			}

			y = y + 1;
			p = p + 2 * dy - 2 * dx;
		}
		else {
			if (visible) {
				pixelBuffer[x + (y * WN_WIDTH)] = 0xFF000000;
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
	V3 line1 = vectorSub(v2, v1);
	V3 line2 = vectorSub(v3, v1);

	// calculate the normal between the lines (the direction the line is facing)
	V3 normal = vectorCrossProduct(line1, line2);

	// normalize the normal as its a direction
	normal.normalize();

	// convert the light to camera space
	V3 L = light.position;

	// apply view transform
	viewTransform(L);

	// apply camera rotation
	rotateV3(L, camera.pitch, camera.yaw);

	// create vertex in the middle to get average diffuse factor TODO: SHOULD CALCULATE FOR EACH VERTEX AND LERP THEM?
	V3 v4((v1.x + v2.x + v3.x) / 3.0f,
		(v1.y + v2.y + v3.y) / 3.0f,
		(v1.z + v2.z + v3.z) / 3.0f);

	// calculate the direction of the light to the vertex
	V3 light_direction = vectorSub(L, v4);
	float light_distance = light_direction.size();
	light_direction.normalize();

	// coordinates get flipped so we must invert the light direction
	light_direction.x *= -1;
	light_direction.y *= -1;
	light_direction.z *= -1;

	// calculate how much the vertex is lit
	float diffuse_factor = vectorDotProduct(light_direction, normal);

	// determine if the face is lit by the lightsource
	if (diffuse_factor < 0.0f) {
		// calculate the light attenuation
		float a = 0.1f / light.strength;
		float b = 0.01f / light.strength;

		float attenuation = 1.0f / (1.0f + (a * light_distance) + (b * light_distance * light_distance));

		return abs(diffuse_factor * attenuation);
	}
	return 0.0f;
}

// TODO: SHADE IN TRIANGLE, LERP VERTEX COLOURS

void Renderer::applyLighting(V3& v1, V3& v2, V3& v3, Colour& base_colour) {
	// little hack for viewpoint lighting, set the first
	// lightsource's position to the camera
	LightSource::sources[0]->position = camera.position;

	// calculate the material colour parts 
	Colour face_colour(0, 0, 0);

	// loop through each lightsource
	for (int l = 0; l < LightSource::sources.size(); ++l) {
		// get the pointer to the lightsource
		LightSource* light = LightSource::sources[l];

		// only calculate if light enabled
		if (light->enabled) {
			// calculate the diffuse factor for the point light with attenuation
			float dp = applyPointLighting(v1, v2, v3, *light);

			// add to the colour
			face_colour.r += dp * light->colour.r * base_colour.r;
			face_colour.g += dp * light->colour.g * base_colour.g;
			face_colour.b += dp * light->colour.b * base_colour.b;

		}
	}
	// scale the colour
	face_colour.r /= 255.0f;
	face_colour.g /= 255.0f;
	face_colour.b /= 255.0f;

	// set the colour of the mesh
	base_colour = face_colour;
}

void Renderer::render() {
	// clear screen
	setBackgroundColour();

	// initialize depth buffer with z-far (maximum z distance)
	std::fill(depthBuffer.begin(), depthBuffer.end(), farPlane);
	
	// actually render the scene
	// loop through each mesh
	for (int m = 0; m < Mesh::meshes.size(); ++m) {
		Mesh* mesh = Mesh::meshes[m]; // get the pointer to the mesh
		
		// loop through each face in mesh
		for (int i = 0; i < mesh->faces.size(); ++i) {
			// define mesh face vertices
			V3 v1(mesh->vertices[mesh->faces[i][0]][0], mesh->vertices[mesh->faces[i][0]][1], mesh->vertices[mesh->faces[i][0]][2]);
			V3 v2(mesh->vertices[mesh->faces[i][1]][0], mesh->vertices[mesh->faces[i][1]][1], mesh->vertices[mesh->faces[i][1]][2]);
			V3 v3(mesh->vertices[mesh->faces[i][2]][0], mesh->vertices[mesh->faces[i][2]][1], mesh->vertices[mesh->faces[i][2]][2]);
			
			// scale vertices to mesh size
			vectorMultBy(v1, mesh->size);
			vectorMultBy(v2, mesh->size);
			vectorMultBy(v3, mesh->size);

			// translate mesh into mesh position
			vectorAddTo(v1, mesh->pos);
			vectorAddTo(v2, mesh->pos);
			vectorAddTo(v3, mesh->pos);

			// rotate mesh face
			rotateMeshFace(v1, v2, v3, mesh->pos, mesh->pitch, mesh->yaw);

			// apply view transformation
			viewTransform(v1);
			viewTransform(v2);
			viewTransform(v3);

			// apply camera rotation
			rotateV3(v1, camera.pitch, camera.yaw);
			rotateV3(v2, camera.pitch, camera.yaw);
			rotateV3(v3, camera.pitch, camera.yaw);

			// cull backfaces
			if (!backfaceCull(v1, v2, v3)) {
				continue; // skip face if on the back of a mesh
			}
			
			// clip triangle against near plane
			std::vector<Triangle> clipped = {}; // clipping may return multiple triangles
			clipTriangle(v1, v2, v3, clipped);
			
			// loop through clipped triangles
			for (int t = 0; t < clipped.size(); ++t) { 
				// apply lighting
				Colour colour = Colour(mesh->colours[i]->toInt());
				bool fill = true;

				// only light filled meshes
				if (fill) {
					// apply point lighting
					if (mesh->absorbsLight) {
						applyLighting(clipped[t].v1, clipped[t].v2, clipped[t].v3, colour);
					}
				}
				
				// project vertices to 2D
				V2 pv1 = project(clipped[t].v1);
				V2 pv2 = project(clipped[t].v2);
				V2 pv3 = project(clipped[t].v3);

				// render the projected triangle
				drawTriangle(pv1, pv2, pv3, colour.toInt(), fill);
			}
		}
	}
	

	// draw crosshair
	// calculate the center of the page (using bit shift for faster divide by 2)
	int center_x = WN_WIDTH >> 1;
	int center_y = WN_HEIGHT >> 1;

	int thickness = 2;

	// draw rectangle
	drawRectangle(center_x - thickness, center_y - thickness, center_x + thickness, center_y + thickness, COLOUR::WHITE.toInt());

	// render the buffer to the screen
	renderBuffer();
}

// private member getters
SDL_Texture* Renderer::getBufferTexture() {
	return bufferTexture;
}