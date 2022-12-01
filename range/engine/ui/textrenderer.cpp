#pragma once
#include "textrenderer.h"

#include <iostream>

bool FontRenderer::setupFont(const std::string& font_path) {
	font = TTF_OpenFont(font_path.c_str(), size);

	if (!font) {
		printf("Failed to open font - %s\n", TTF_GetError());
		return false;
	}
	return true;
}

bool FontRenderer::setupCharacterSurfaces() {
	std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!#$%&'()*+,-./:;<=>?@[]^_`{|}~ ";
	std::string temp;
	for (std::string::size_type i = 0; i < characters.size(); ++i) {
		temp = characters[i];
		SDL_Surface* s = TTF_RenderText_Solid(font, temp.c_str(), colour.toSDL());
		if (s == nullptr) {
			printf("Surface is nullptr in setupCharacterSurfaces().\n");
			return false;
		}
		characterSurfaces[temp] = s;
	}

	return true;
}

bool FontRenderer::setup(const std::string& font_path) {
	if (!setupFont(font_path)) {
		std::cout << "Failed FontRenderer::setupFont(font_path) " << font_path << std::endl;
		return false;
	}
	if (!setupCharacterSurfaces()) {
		std::cout << "Failed FontRenderer::setupCharacterSurfaces()" << std::endl;
		return false;
	}

	return true;
}

void FontRenderer::render(RenderSurface& render_surface, const std::string& str, const V2& position) {
	// set initial position
	int x = position.x;

	// render each character from the cached surfaces
	for (std::string::size_type i = 0; i < str.size(); ++i) {
		SDL_Surface* s = characterSurfaces[std::string(1, str[i])];

		s->clip_rect.x = x;
		s->clip_rect.y = position.y;

		x += s->clip_rect.w;

		// draw the char to the render surface
		SDL_BlitSurface(s, NULL, render_surface.windowSurface, &s->clip_rect);
	}
}

FontRenderer::~FontRenderer() {
	// free each prerendered surface
	std::map<std::string, SDL_Surface*>::iterator it;
	for (it = characterSurfaces.begin(); it != characterSurfaces.end(); it++) {
		SDL_FreeSurface(it->second);
	}

	TTF_CloseFont(font);

	characterSurfaces.clear();
}

bool KeyValueFontRenderer::createKey(const std::string& key) {
	// generate key surface
	keySurfaces[key] = TTF_RenderText_Solid(font, key.c_str(), colour.toSDL());

	if (!keySurfaces[key]) {
		std::cout << "Failed to setupKeySurface(), surface was nullptr." << std::endl;
		return false;
	}

	return true;
}

void KeyValueFontRenderer::render(RenderSurface& render_surface, const std::string& key, const std::string& value, const V2& position) {
	// get the corresponding surface
	SDL_Surface* key_surface = keySurfaces[key];

	// set the text surface rect position
	key_surface->clip_rect.x = position.x;
	key_surface->clip_rect.y = position.y;

	// draw the cached key text
	SDL_BlitSurface(key_surface, NULL, render_surface.windowSurface, &key_surface->clip_rect);

	// then call the base render method with an updated position
	FontRenderer::render(render_surface, value, V2(position.x + key_surface->clip_rect.w, position.y));
}

KeyValueFontRenderer::~KeyValueFontRenderer() {
	// base destructor already called
	
	// free each prerendered key
	std::map<std::string, SDL_Surface*>::iterator it;
	for (it = keySurfaces.begin(); it != keySurfaces.end(); it++) {
		SDL_FreeSurface(it->second);
	}

	keySurfaces.clear();
}