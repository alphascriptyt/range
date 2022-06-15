#pragma once

#include <stdint.h>
#include <algorithm>
#include "sdl.h"

class Colour {
	void clampComponents();

public:
	int r = 0;
	int g = 0;
	int b = 0;

	Colour();
	Colour(uint8_t _r, uint8_t _g, uint8_t _b);
	Colour(uint32_t _colour);

	uint32_t toInt();
	void print();
	SDL_Colour toSDL();
};

class DiffuseColour {
public:
	float r = 0;
	float g = 0;
	float b = 0;

	DiffuseColour();
	DiffuseColour(float _r, float _g, float _b);
};

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
