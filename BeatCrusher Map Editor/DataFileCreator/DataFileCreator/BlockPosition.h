#pragma once
#include <iostream>
#include <SDL.h>
#include <vector>
#include "Button.h"
using namespace std;
class BlockPosition {
private:
	vector<SDL_Point> pos;
	vector<SDL_Point> dir;
	Button left, right;
	SDL_Texture *page_tex;
	int show_page;
	static SDL_Texture* arrow[9];

public:
	BlockPosition(SDL_Renderer *renderer);
	~BlockPosition();
	void Rendering(SDL_Renderer *renderer, bool playing, int alpha = -1);
	void Event(SDL_Event event, SDL_Renderer *renderer);
	void SetSize(int size, SDL_Renderer *renderer);
	void ChangePage(SDL_Renderer *renderer, int n = -1);
	void SavePos(FILE *f);
	void LoadPos(FILE *f);
	void SaveDir(FILE *f);
	void LoadDir(FILE *f);
	
	static void Init(SDL_Renderer *renderer);
	static void Destroy();
};

