#include "MouseEffect.h"
#include "LoadTexture.h"
#include "SetDataFile.h"
#include <iostream>
using namespace std;

extern double flexible;

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

MouseEffect::MouseEffect(SDL_Renderer *renderer) {
	this->length = 100;
	this->pos.resize(this->length);
	for (int i = 0; i < this->length; i++) {
		SDL_GetMouseState(&this->pos[i].x, &this->pos[i].y);
	}
	this->cursur = LoadImage(Location(RES, "", "circle.img").c_str(), renderer);
	this->begin_point = 0;
}

MouseEffect::~MouseEffect() {
	this->begin_point = 0;
	this->pos.clear();
}

void MouseEffect::Rendering(SDL_Renderer * renderer) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	for (int i = this->length - 1; i > 0; i--) {
		int p = i + this->begin_point;
		if (p >= this->length)
			p -= this->length;
		int p2 = p - 1;
		if (p2 < 0)
			p2 = this->length - 1;
		if (this->pos[p].x != -1) {
			//SDL_SetRenderDrawColor(renderer, 255, 255, 255, (i + 1) * 2 + 55);
			//SDL_RenderDrawLine(renderer, this->pos[p].x, this->pos[p].y, this->pos[p2].x, this->pos[p2].y);
			
			DrawLine(this->pos[p], this->pos[p2], renderer, 15 * flexible, 255, 255, 255, (float)i * 200.f / (float)this->length);
		}
	}

	SDL_Rect r;
	r.w = 60 * flexible;
	r.h = r.w;
	int p;
	if (this->begin_point)
		p = this->begin_point - 1;
	else
		p = this->pos.size() - 1;
	r.x = this->pos[p].x - r.w / 2;
	r.y = this->pos[p].y - r.h / 2;
	SDL_RenderCopy(renderer, this->cursur, NULL, &r);
}

void MouseEffect::SetPoint() {
	SDL_Point point;
	SDL_GetMouseState(&point.x, &point.y);
	this->pos[this->begin_point] = point;
	this->begin_point++;
	if (this->begin_point >= this->length)
		this->begin_point = 0;
}

void MouseEffect::SetLength(int l) {
	this->length = l;
}

void MouseEffect::GetMousePosition(SDL_Point * p1, SDL_Point * p2, int n) {
	if(this->begin_point)
		*p1 = this->pos[this->begin_point - 1];
	else
		*p1 = this->pos[this->pos.size() - 1];
	
	for (int i = this->begin_point - 1; i != this->begin_point; i--) {
		if (i < 0)
			i = this->pos.size() - 1;
		if (p1->x != this->pos[i].x || p1->y != this->pos[i].y) {
			*p2 = this->pos[i];
			if (!n) {
				break;
			}
			else {
				*p1 = *p2;
				n--;
			}
		}
		if (this->begin_point == i)
			break;
	}
}
