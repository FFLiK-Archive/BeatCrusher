#include "Button.h"
#include "LoadTexture.h"
#include <SDL_mixer.h>
#include "SetDataFile.h"

extern double flexible;
extern int volume;


Button::Button(SDL_Renderer * renderer, string str, SDL_Point pos, int font_size, bool line, int r, int g, int b, int a) {
	this->click_eff = Mix_LoadWAV(Location(RES, "", "click.eff").c_str());
	this->button_t = LoadText(str.c_str(), renderer, font_size * flexible, "main_font", r, g, b);
	this->text_btn = true;
	this->line = line;
	this->mouse_on = false;
	this->pos = pos;
	this->pushed = false;
	this->a = a;
	SDL_SetTextureAlphaMod(this->button_t, a);
}

Button::Button(SDL_Renderer * renderer, string str, SDL_Point pos) {
	this->click_eff = Mix_LoadWAV(Location(RES, "", "click.eff").c_str());
	this->button_t = LoadImage(str.c_str(), renderer);
	this->text_btn = false;
	this->line = false;
	this->mouse_on = false;
	this->pos = pos;
	this->pushed = false;
}

Button::~Button() {
	Mix_FreeChunk(this->click_eff);
	if (this->button_t)
		SDL_DestroyTexture(this->button_t);
	this->button_t = nullptr;
}

void Button::MoveButton(SDL_Point pos) {
	this->pos = pos;
}

void Button::Rendering(SDL_Renderer * renderer) {
	int alpha = 0;
	if (this->pushed) {
		alpha = a - 155;
		if (alpha < 0)
			alpha = 0;
		SDL_SetTextureAlphaMod(this->button_t, alpha);
	}
	else if (this->mouse_on) {
		alpha = a;
		SDL_SetTextureAlphaMod(this->button_t, alpha);
	}
	else {
		alpha = a - 55;
		if (alpha < 0)
			alpha = 0;
		SDL_SetTextureAlphaMod(this->button_t, alpha);
	}
	SDL_Rect dst;
	SDL_QueryTexture(this->button_t, NULL, NULL, &dst.w, &dst.h);
	if (!this->text_btn) {
		dst.w *= flexible;
		dst.h *= flexible;
	}
	dst.x = this->pos.x * flexible;
	dst.y = this->pos.y * flexible;
	dst.x -= dst.w / 2;
	dst.y -= dst.h / 2;
	SDL_RenderCopy(renderer, this->button_t, NULL, &dst);
	if (line) {
		dst.x -= 10 * flexible;
		dst.y -= 10 * flexible;
		dst.w += 20 * flexible;
		dst.h += 20 * flexible;
		SDL_Point p1, p2, p3, p4;
		p1 = { dst.x, dst.y };
		p2 = { dst.x + dst.w, dst.y };
		p3 = { dst.x + dst.w, dst.y + dst.h };
		p4 = { dst.x, dst.y + dst.h };
		DrawLine(p1, p2, renderer, 15 * flexible, 255, 255, 255, alpha / 5);
		DrawLine(p2, p3, renderer, 15 * flexible, 255, 255, 255, alpha / 5);
		DrawLine(p3, p4, renderer, 15 * flexible, 255, 255, 255, alpha / 5);
		DrawLine(p4, p1, renderer, 15 * flexible, 255, 255, 255, alpha / 5);
	}
}

bool Button::CheckClick(SDL_Event event) {
	SDL_Rect dst;
	SDL_QueryTexture(this->button_t, NULL, NULL, &dst.w, &dst.h);
	if (!this->text_btn) {
		dst.w *= flexible;
		dst.h *= flexible;
	}
	dst.x = this->pos.x * flexible;
	dst.y = this->pos.y * flexible;
	dst.x -= dst.w / 2;
	dst.y -= dst.h / 2;

	if (event.type == SDL_MOUSEMOTION) {
		if (event.motion.x >= dst.x && event.motion.x <= dst.x + dst.w
			&& event.motion.y >= dst.y && event.motion.y <= dst.y + dst.h) {
			this->mouse_on = true;
		}
		else {
			this->pushed = false;
			this->mouse_on = false;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN && this->mouse_on) {
		this->pushed = true;
	}
	else if (event.type == SDL_MOUSEBUTTONUP && this->pushed) {
		this->pushed = false;
		Mix_VolumeChunk(this->click_eff, volume * 128 / 100);
		Mix_PlayChannel(-1, this->click_eff, 0);
		return true;
	}
	return false;
}
