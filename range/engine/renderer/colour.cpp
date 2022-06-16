#include "colour.h"
#include <iostream>

// constructors
Colour::Colour() {
	r = 0;
	g = 0;
	b = 0;
}

Colour::Colour(uint8_t _r, uint8_t _g, uint8_t _b) {
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
void Colour::clampComponents() {
	// method for clamping R,G,B from 0-255

	// clamp r
	if (r > 255) { r = 255; }
	else if (r < 0) { r = 0; }

	// clamp g
	if (g > 255) { g = 255; }
	else if (g < 0) { g = 0; }

	// clamp b
	if (b > 255) { b = 255; }
	else if (b < 0) { b = 0; }
}

uint32_t Colour::toInt() {
	// return an integer representation of RGB
	clampComponents();
	
	// pack into XRGB format to match SDL surface
	return (r << 16) | (g << 8) | b;
}

void Colour::print() {
	// print the colour
	std::cout << (int)r << " " << (int)g << " " << (int)b << std::endl;
}

SDL_Colour Colour::toSDL() {
	// helper method to convert the colour to SDL (mainly for KISS_SDL use)
	SDL_Colour c = { 0 };
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = 255;

	return c;
}

// predefined colours
namespace COLOUR {
	Colour WHITE(255, 255, 255);
	Colour SILVER(191, 191, 191);
	Colour GRAY(128, 128, 128);
	Colour BLACK(0, 0, 0);
	Colour RED(255, 0, 0);
	Colour MAROON(128, 0, 0);
	Colour LIME(0, 255, 0);
	Colour GREEN(0, 128, 0);
	Colour BLUE(0, 0, 255);
	Colour YELLOW(255, 255, 0);
	Colour OLIVE(128, 128, 0);
	Colour AQUA(0, 255, 255);
	Colour TEAL(0, 128, 128);
	Colour NAVY(0, 0, 128);
	Colour FUCHSIA(255, 0, 255);
	Colour PURPLE(128, 0, 128);
	Colour GOLD(255, 215, 0);
}
