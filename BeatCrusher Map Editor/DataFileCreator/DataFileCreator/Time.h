#pragma once
#include <SDL.h>
#include "Button.h"
#include "BlockPosition.h"
class Time {
private:
	SDL_Rect r;
	SDL_Texture *time_t, *loop_t, *num_t;
public:
	double time;
	int pos;
	int loop;
	BlockPosition *block_pos;

	Time(int pos, SDL_Renderer *renderer);
	~Time();
	void ChangeTime(double t, SDL_Renderer *renderer);
	void ChangeLoop(int l, SDL_Renderer *renderer);
	void ChangeNum(int n, SDL_Renderer *renderer);
	bool Event(SDL_Event event);
	void Rendering(SDL_Renderer *renderer, bool selected);
};