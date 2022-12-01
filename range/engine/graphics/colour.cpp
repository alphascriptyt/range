#include "colour.h"
#include <iostream>

// constructors
Colour::Colour() {
	r = 0;
	g = 0;
	b = 0;
}

Colour::Colour(float _r, float _g, float _b) {
	r = _r;
	g = _g;
	b = _b;
}

Colour::Colour(uint32_t _colour) {
	// extract RGB from XRGB colour (ignore first byte)
	r = (_colour >> 16) & 0xff;
	g = (_colour >> 8) & 0xff;
	b = _colour & 0xff;
}

// methods
uint32_t Colour::toInt() {
	// return an integer representation of RGB

	// TODO: Make video part on this, all I need to do is remove clamping the components and its twice as fast.
	
	// pack into XRGB format to match SDL surface and scale up to 0-255
	return ((int)(r * 255) << 16) |
		((int)(g * 255) << 8) |
		((int)(b * 255));
}

void Colour::print() {
	// print the colour
	std::cout << r << " " << g << " " << b << std::endl;
}

SDL_Colour Colour::toSDL() {
	// helper method to convert the colour to SDL

	// set the colour components and scale each up to 0-255
	SDL_Colour c = { 0 };
	c.r = r * 255;
	c.g = g * 255;
	c.b = b * 255;
	c.a = 255;

	return c;
}

// predefined colours
namespace COLOUR {
	Colour WHITE(1.0f, 1.0f, 1.0f);
	Colour SILVER(0.75f, 0.75f, 0.75f);
	Colour GRAY(0.5f, 0.5f, 0.5f);
	Colour BLACK(0, 0, 0);
	Colour RED(1.0f, 0, 0);
	Colour MAROON(0.5f, 0, 0);
	Colour LIME(0, 1.0f, 0);
	Colour GREEN(0, 0.5f, 0);
	Colour BLUE(0, 0, 1.0f);
	Colour YELLOW(1.0f, 1.0f, 0);
	Colour OLIVE(0.5f, 0.5f, 0);
	Colour AQUA(0, 1.0f, 1.0f);
	Colour TEAL(0, 0.5f, 0.5f);
	Colour NAVY(0, 0, 0.5f);
	Colour FUCHSIA(1.0f, 0, 1.0f);
	Colour PURPLE(0.5f, 0, 0.5f);
	Colour GOLD(1.0f, 0.85f, 0);
}
