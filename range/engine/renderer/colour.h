#pragma once

#include <stdint.h>
#include <algorithm>
#include "sdl.h"

class Colour {
	void clampComponents();

public:
	// components
	int r = 0;
	int g = 0;
	int b = 0;

	// constructors
	Colour();
	Colour(uint8_t _r, uint8_t _g, uint8_t _b);
	Colour(uint32_t _colour); // unpack XRGB into R,G,B components

	// methods
	uint32_t toInt();		// pack as XRGB int
	void print();			// print R,G,B components
	SDL_Colour toSDL();		// convert to SDL_Colour type
};

// predefined colours
namespace COLOUR {
	extern Colour WHITE;
	extern Colour SILVER;
	extern Colour GRAY;
	extern Colour BLACK;
	extern Colour RED;
	extern Colour MAROON;
	extern Colour LIME;
	extern Colour GREEN;
	extern Colour BLUE;
	extern Colour YELLOW;
	extern Colour OLIVE;
	extern Colour AQUA;
	extern Colour TEAL;
	extern Colour NAVY;
	extern Colour FUCHSIA;
	extern Colour PURPLE;
	extern Colour GOLD;
}
