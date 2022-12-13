#pragma once

#include <vector>
#include "SDL.h"
#include "timer.h"
class RenderSurface {
public:
	SDL_Window* window = nullptr;
	std::vector<float> depthBuffer; // depth of pixels

	inline bool testAndSetDepth(int i, float depth) {
		// overwrite pixel in buffer if closer to camera
		if (depthBuffer[i] > depth) {
			depthBuffer[i] = depth;

			return true;	// draw pixel
		}
		return false;		// don't draw pixel
	}

public:
	SDL_Surface* windowSurface = nullptr;	// SDL render target
	uint32_t* pixels = nullptr;				// pixel buffer

	int length = 0;
	int width = 0;
	int height = 0;

	// constructor
	RenderSurface() {}

	// methods
	bool init(SDL_Window* wn, int w, int h, int colour, float depth = -1.0f) {
		window = wn;

		// try get a pointer to the SDL window surface
		windowSurface = SDL_GetWindowSurface(window);

		// check for errors
		if (!windowSurface) {
			printf("Failed to GetWindowSurface: %s\n", SDL_GetError());
			return false;
		}

		// get access to the pixel buffer 
		pixels = (uint32_t*)windowSurface->pixels;

		resize(w, h, colour, depth);

		return true;
	}

	inline void resize(int w, int h, int colour, float depth = -1.0f) {
		width = w;
		height = h;

		// calculate the buffer length incase it has changed
		length = width * height;

		// clear the depth buffer
		depthBuffer = std::vector<float>();

		// initialize depth buffer with given value (z-far)
		for (int i = 0; i < length; ++i) {
			depthBuffer.push_back(depth);
		}

		// TODO: re-create pixels somehow? probably using new surface
	}
	inline void setIndex(int i, int colour, float depth = -1.0f) {
		if (testAndSetDepth(i, depth)) {
			pixels[i] = colour;
		}
	}
	inline void setPixel(int x, int y, int colour, float depth = -1.0f) {
		int i = x + width * y;

		if (testAndSetDepth(i, depth)) {
			pixels[i] = colour;
		}
	}
	inline void clear(int colour, float depth = -1.0f) {	
		SDL_FillRect(windowSurface, NULL, colour); // much faster than std::fill somehow
		std::fill(depthBuffer.begin(), depthBuffer.end(), depth); // TODO: is farPlane working right?
	}

	inline SDL_Surface* getSurfaceSDL() { return windowSurface; }
};