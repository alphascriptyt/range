#pragma once

#include <stdint.h>
#include <algorithm>
#include "sdl.h"

class Colour {
	void clampComponents();

public:
	// RGB components represented from 0-1
	float r = 0;
	float g = 0;
	float b = 0;

	// constructors
	Colour();
	Colour(float _r, float _g, float _b);
	Colour(uint32_t _colour); // unpack XRGB into R,G,B components

	// methods
	uint32_t toInt();		// pack as XRGB int
	void print();			// print R,G,B components
	SDL_Colour toSDL();		// convert to SDL_Colour tgpe

	// operator overloading
	inline Colour operator+(const Colour & v) {
		return Colour(
			r + v.r,
			g + v.g,
			b + v.b
		);
	}
	inline Colour operator+(const float n) {
		return Colour(
			r + n,
			g + n,
			b + n
		);
	}
	inline void operator+=(const Colour& v) {
		r += v.r;
		g += v.g;
		b += v.b;
	}
	inline void operator+=(const float n) {
		r += n;
		g += n;
		b += n;
	}

	inline Colour operator-(const Colour& v) {
		return Colour(
			r - v.r,
			g - v.g,
			b - v.b
		);
	}
	inline Colour operator-(const float n)
	{
		return Colour(
			r - n,
			g - n,
			b - n
		);
	}
	inline void operator-=(const Colour& v) {
		r -= v.r;
		g -= v.g;
		b -= v.b;
	}
	inline void operator-=(const float n) {
		r -= n;
		g -= n;
		b -= n;
	}

	inline Colour operator*(const Colour& v) {
		return Colour(
			r * v.r,
			g * v.g,
			b * v.b
		);
	}
	inline Colour operator*(const float n) {
		return Colour(
			r * n,
			g * n,
			b * n
		);
	}
	inline void operator*=(const Colour& v) {
		r *= v.r;
		g *= v.g;
		b *= v.b;
	}
	inline void operator*=(const float n) {
		r *= n;
		g *= n;
		b *= n;
	}

	inline Colour operator/(const Colour& v) {
		return Colour(
			r / v.r,
			g / v.g,
			b / v.b
		);
	}
	inline Colour operator/(const float n) {
		return Colour(
			r / n,
			g / n,
			b / n
		);
	}
	inline void operator/=(const Colour& v) {
		r /= v.r;
		g /= v.g;
		b /= v.b;
	}
	inline void operator/=(const float n) {
		r /= n;
		g /= n;
		b /= n;
	}

	inline void operator=(const float n) {
		r = n;
		g = n;
		b = n;
	}
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
