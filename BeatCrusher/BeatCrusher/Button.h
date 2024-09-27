#pragma once
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
using namespace std;

class Button {
private:
	SDL_Texture *button_t;
	SDL_Point pos;
	bool pushed;
	bool mouse_on;
	bool text_btn;
	bool line;
	int a;

	Mix_Chunk *click_eff;

public:
	Button(SDL_Renderer *renderer, string str, SDL_Point pos, int font_size, bool line, int r, int g, int b, int a = 255);
	Button(SDL_Renderer *renderer, string str, SDL_Point pos);
	~Button();

	void MoveButton(SDL_Point pos);
	void Rendering(SDL_Renderer *renderer);
	bool CheckClick(SDL_Event event);
};

