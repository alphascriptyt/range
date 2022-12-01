#pragma once

#include "SDL.h"
#include "SDL_ttf.h"
#include "vec2D.h"
#include "../graphics/colour.h"
#include "../graphics/rendersurface.h"
#include "uicomponent.h"

#include <string>
#include <map>

// class for rendering text using prerendered characters
class FontRenderer {
protected:
	TTF_Font* font = nullptr;
	std::map<std::string, SDL_Surface*> characterSurfaces;
	int size = 0;
	Colour colour;

	bool setupFont(const std::string& font_path);
	bool setupCharacterSurfaces();
	bool setup(const std::string& font_path);

public:
	FontRenderer(const std::string& font_path, const Colour& _colour, int _size) {
		colour = _colour;
		size = _size;

		setup(font_path);
	}

	virtual ~FontRenderer();

	void render(RenderSurface& render_surface, const std::string& str, const V2& position);
};
 
// class for efficient rendering of text with a value
class KeyValueFontRenderer : FontRenderer {
	std::map<std::string, SDL_Surface*> keySurfaces;

public:
	KeyValueFontRenderer(const std::string& font_path, const Colour& _colour, int _size) : FontRenderer(font_path, _colour, _size) {}

	~KeyValueFontRenderer();

	bool createKey(const std::string& key);
	void render(RenderSurface& render_surface, const std::string& key, const std::string& value, const V2& position);
};
