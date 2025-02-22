#include "LoadTexture.h"
#include "han2unicode.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
SDL_Texture * LoadImage(const char * filename, SDL_Renderer * renderer)
{
	SDL_Surface *surface;
	SDL_Texture *texture;
	surface = IMG_Load(filename);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture * LoadText(const char * str, SDL_Renderer * renderer, int size, const char *fontfile_name, int r, int g, int b)
{
	TTF_Font* font;
	SDL_Surface *surface;
	SDL_Texture *texture;
	string adr;
	adr = "font\\";
	adr += fontfile_name;
	adr += ".C_font";
	font = TTF_OpenFont(adr.c_str(), size);

	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 255;

	Uint16 unicode[32768];
	han2unicode(str, unicode);

	surface = TTF_RenderUNICODE_Blended(font, unicode, color);

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	//delete font;
	TTF_CloseFont(font);
	return texture;
}

SDL_Texture * LoadText(const wstring str, SDL_Renderer * renderer, int size, const char * fontfile_name)
{
	TTF_Font* font;
	SDL_Surface *surface;
	SDL_Texture *texture;
	string adr;
	adr = "Font\\";
	adr += fontfile_name;
	adr += ".C_font";
	font = TTF_OpenFont(adr.c_str(), size);

	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	surface = TTF_RenderUNICODE_Blended(font, (Uint16*)str.c_str(), color);

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	//delete font;
	TTF_CloseFont(font);
	return texture;
}
