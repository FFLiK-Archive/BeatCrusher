#pragma once
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
using namespace std;
class MouseEffect {
private:
	vector<SDL_Point> pos;
	int begin_point;
	int length;
	SDL_Texture *cursur;

public:
	MouseEffect(SDL_Renderer *renderer);
	~MouseEffect();
	void Rendering(SDL_Renderer *renderer);
	void SetPoint();
	void SetLength(int l);
	void GetMousePosition(SDL_Point *p1, SDL_Point *p2, int n);
};

