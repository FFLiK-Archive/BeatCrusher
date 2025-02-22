#pragma once
#include <SDL.h>
#include "Button.h"
class TimeEditer {
private:
	bool time_change = true;
	Button time_btn;
	Button loop_btn;
	Button apply_btn;
	SDL_Texture *time_t, *loop_t;
	int time_pos;
	bool dot;
	int time_i;
	string time_d;
	int loop;
public:
	TimeEditer(SDL_Renderer* renderer);
	~TimeEditer();
	double GetTime();
	int GetLoop();
	int SetTime(double t, SDL_Renderer *renderer);
	int SetLoop(int l, SDL_Renderer *renderer);
	bool Event(SDL_Event event, SDL_Renderer *renderer);
	void Rendering(SDL_Renderer* renderer);
};

