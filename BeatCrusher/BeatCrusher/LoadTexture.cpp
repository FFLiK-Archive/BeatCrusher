#include "LoadTexture.h"
#include "han2unicode.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include "SetDataFile.h"
SDL_Texture* circle; 

void QuitLoadTextureLibrary() {
	SDL_DestroyTexture(circle);
}

void DrawLine(SDL_Point begin, SDL_Point end, SDL_Renderer * renderer, int circle_size, int r, int g, int b, int a) {
	if (!circle)
		circle = LoadImage(Location(RES, "", "circle.img").c_str(), renderer);
	SDL_SetTextureAlphaMod(circle, a);
	SDL_SetTextureColorMod(circle, r, g, b);
	int addX, addY;
	int counter = 0;
	int dx = end.x - begin.x;
	int dy = end.y - begin.y;
	if (dx < 0) {
		addX = -1;
		dx = -dx;
	}
	else {
		addX = 1;
	}
	if (dy < 0) {
		addY = -1;
		dy = -dy;
	}
	else {
		addY = 1;
	}

	int x = begin.x;
	int y = begin.y;

	if (dx >= dy) {
		for (int i = 0; i < dx; i++) {
			x += addX;
			counter += dy;
			if (counter >= dx) {
				y += addY;
				counter -= dx;
			}
			SDL_Rect dst;
			dst.w = circle_size;
			dst.h = circle_size;
			dst.x = x - circle_size / 2;
			dst.y = y - circle_size / 2;
			SDL_RenderCopy(renderer, circle, NULL, &dst);
		}
	}
	else {
		for (int i = 0; i < dy; i++) {
			y += addY;
			counter += dx;
			if (counter >= dy) {
				x += addX;
				counter -= dy;
			}
			SDL_Rect dst;
			dst.w = circle_size;
			dst.h = circle_size;
			dst.x = x - circle_size / 2;
			dst.y = y - circle_size / 2;
			SDL_RenderCopy(renderer, circle, NULL, &dst);
		}
	}
}
SDL_Texture * LoadImage(const char * filename, SDL_Renderer * renderer)
{
	SDL_Surface *surface;
	SDL_Texture *texture;
	surface = IMG_Load(filename);
	cout << IMG_GetError();
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
	adr = fontfile_name;
	adr += ".font";
	adr = Location(RES, "", adr);
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
	adr = fontfile_name;
	adr += ".font";
	adr = Location(RES, "", adr);
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
