#include "MenuProcessor.h"
#include "LoadTexture.h"
#include "Background.h"
#include "SetDataFile.h"
#include <ctime>

extern double flexible;
extern string music_to_play;
extern string version;
extern int win_w, win_h;
extern int volume;
extern int offset;

#define MAX(x, y) (x > y ? x : y)

#define EFFECT_MAX 30

MenuProcessor::MenuProcessor(SDL_Renderer * renderer, SDL_Event *event) {
	this->title_t = LoadText("Beat Crusher", renderer, 200 * flexible, "main_font", 255, 255, 255);
	this->version_t = LoadText(version.c_str(), renderer, 20 * flexible, "main_font", 255, 255, 255);
	SDL_SetTextureAlphaMod(this->version_t, 50);
	this->close_effect_cnt = 0;
	this->open_effect_cnt = EFFECT_MAX;
	this->close_btn = new Button(renderer, "EXIT >", { 1400, 800 }, 50, false, 255, 255, 255);
	this->music_list_btn = new Button(renderer, "< MUSIC", { 200, 800 }, 50, false, 255, 255, 255);
	this->start_btn = new Button(renderer, "START", { 800, 700 }, 100, false, 255, 255, 255);
	this->setting_btn = new Button(renderer, "Setting", {1500, 50}, 30, true, 255, 255, 255);
	this->finish = false;
	this->dead_message = "";
	this->setting = false;

	this->renderer = renderer;
	this->event = event;

	Background::NewBackground("std");
	this->music = Mix_LoadMUS(Location(RES, "", "music.msc").c_str());

	Mix_PlayMusic(this->music, -1);

	this->background_effect_clock = clock();
	this->background_effect = 0;
}

MenuProcessor::~MenuProcessor() {
	delete this->close_btn;
	delete this->music_list_btn;
	delete this->start_btn;
	SDL_DestroyTexture(this->title_t);
	this->title_t = nullptr;
	Mix_HaltMusic();
	Mix_FreeMusic(this->music);
}

void MenuProcessor::Rendering() {
	int move = MAX(this->close_effect_cnt * this->close_effect_cnt, this->open_effect_cnt * this->open_effect_cnt) * flexible;

	SDL_Rect r;
	SDL_QueryTexture(this->title_t, NULL, NULL, &r.w, &r.h);
	r.w *= (this->background_effect) * flexible / 800 + 1;
	r.h *= (this->background_effect) * flexible / 800 + 1;
	r.x = 800 * flexible - r.w / 2;
	r.y = 100 * flexible - move;

	SDL_RenderCopy(renderer, this->title_t, NULL, &r);

	SDL_QueryTexture(this->version_t, NULL, NULL, &r.w, &r.h);
	r.x = 1580 * flexible - r.w;
	r.y = 880 * flexible + move - r.h;
	SDL_RenderCopy(renderer, this->version_t, NULL, &r);

	this->close_btn->MoveButton({ 1400, 800 + move });
	this->music_list_btn->MoveButton({ 200, 800 + move });
	this->start_btn->MoveButton({ 800, 700 + move });
	this->setting_btn->MoveButton({ 1500, 50 - move });

	this->close_btn->Rendering(renderer);
	this->music_list_btn->Rendering(renderer);
	this->start_btn->Rendering(renderer);
	this->setting_btn->Rendering(renderer);

	if (dead_message == "EXIT") {
		r.w = win_w;
		r.h = win_h;
		r.x = 0;
		r.y = 0;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, this->close_effect_cnt * 255 / EFFECT_MAX);
		SDL_RenderFillRect(renderer, &r);
	}

	if (this->setting) {
		Setting::Rendering(renderer);
	}
}

void MenuProcessor::Processing_Uniform() {
	
	if (this->close_effect_cnt)
		this->close_effect_cnt += 0.4;

	if (this->close_effect_cnt > EFFECT_MAX)
		this->finish = true;

	if (this->open_effect_cnt > 0)
		this->open_effect_cnt -= 0.4;
	else
		this->open_effect_cnt = 0;
	
	if (clock() - this->background_effect_clock > 1140) {
		this->background_effect_clock += 1140;
		this->background_effect = 60;
	}
	
	if (this->background_effect && !this->close_effect_cnt && !this->open_effect_cnt) {
		Background::ChangeAlpha(30 - abs(this->background_effect / 2 - 30) + 5);
		this->background_effect--;
	}
}

void MenuProcessor::Processing_Erratic() {
	if (!this->close_effect_cnt && !this->open_effect_cnt) {
		if (!this->setting) {
			if (this->close_btn->CheckClick(*this->event)) {
				this->close_effect_cnt = 1;
				Mix_FadeOutMusic((EFFECT_MAX - 10) * 15);
				this->dead_message = "EXIT";
			}
			else if (this->music_list_btn->CheckClick(*this->event)) {
				this->close_effect_cnt = 1;
				Mix_FadeOutMusic((EFFECT_MAX - 10) * 15);
				this->dead_message = "MUSIC_LIST";
				Background::ChangeAlpha(5);
			}
			else if (this->start_btn->CheckClick(*this->event)) {
				this->close_effect_cnt = 1;
				Mix_FadeOutMusic((EFFECT_MAX - 10) * 15);
				this->dead_message = "START";
				Background::ChangeAlpha(5);
			}
			else if (this->setting_btn->CheckClick(*this->event)) {
				Setting::Init(this->renderer);
				this->setting = true;
			}
		}
		else {
			Setting::Processing(this->event, this->renderer);
			this->setting = Setting::CheckSettingEditing();
			if (!this->setting) {
				SDL_RWops* v_f;
				v_f = SDL_RWFromFile(Location(DAT, "", "volume.dat").c_str(), "w");
				WriteInt(v_f, volume);
				SDL_RWclose(v_f);

				v_f = SDL_RWFromFile(Location(DAT, "", "offset.dat").c_str(), "w");
				WriteInt(v_f, offset);
				SDL_RWclose(v_f);
				Setting::Destroy();
			}
		}
	}
}

bool MenuProcessor::Finished() {
	return this->finish;
}

string MenuProcessor::GetDeadMessage() {
	return dead_message;
}

int Setting::open_effect_cnt, Setting::close_effect_cnt;
string Setting::return_msg;
SDL_Texture *Setting::volume_t, *Setting::title_t, *Setting::volume_text_t;
SDL_Texture *Setting::offset_t, *Setting::offset_text_t;
Button *Setting::close_btn;
bool Setting::editing;
bool Setting::volume_editing, Setting::offset_editing;

#define EFFECT_MAX 30

void Setting::ChangeVolume(int v, SDL_Renderer *renderer) {
	SDL_DestroyTexture(volume_t);
	volume = v;
	volume_t = LoadText((to_string(volume) + "%").c_str(), renderer, 40 * flexible, "main_font", 255, 255, 255);
	Mix_VolumeMusic(volume * 128 / 100);
}

void Setting::ChangeOffset(int v, SDL_Renderer* renderer) {
	SDL_DestroyTexture(offset_t);
	offset = v;
	offset_t = LoadText((to_string(offset) + "ms").c_str(), renderer, 40 * flexible, "main_font", 255, 255, 255);
}

void Setting::Init(SDL_Renderer * renderer) {
	open_effect_cnt = EFFECT_MAX;
	close_effect_cnt = 0;
	volume_t = LoadText((to_string(volume) + "%").c_str(), renderer, 40 * flexible, "main_font", 255, 255, 255);
	offset_t = LoadText((to_string(offset) + "ms").c_str(), renderer, 40 * flexible, "main_font", 255, 255, 255);
	volume_text_t = LoadText("Volume", renderer, 40 * flexible, "main_font", 255, 255, 255);
	offset_text_t = LoadText("Offset", renderer, 40 * flexible, "main_font", 255, 255, 255);
	title_t = LoadText("Setting", renderer, 60 * flexible, "main_font", 255, 255, 255);
	close_btn = new Button(renderer, "CLOSE", { 800, 650 }, 30, true, 255, 255, 255, 255);
	editing = true;
	volume_editing = false;
}

void Setting::Destroy() {
	SDL_DestroyTexture(volume_t);
	SDL_DestroyTexture(volume_text_t);
	SDL_DestroyTexture(title_t);
	delete close_btn;
}

void Setting::Rendering(SDL_Renderer * renderer) {
	int m = 0;
	int a = 0;
	if (open_effect_cnt) {
		m = open_effect_cnt;
		open_effect_cnt--;
	}
	else if (close_effect_cnt) {
		m = close_effect_cnt;
		close_effect_cnt++;
	}
	a = m;
	m = m * m * flexible;

	SDL_Rect r;
	r.w = win_w;
	r.h = win_h;
	r.x = 0;
	r.y = 0;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150 - a * 150 / EFFECT_MAX);
	SDL_RenderFillRect(renderer, &r);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
	r.w = 800 * flexible;
	r.h = 500 * flexible;
	r.x = win_w / 2 - r.w / 2;
	r.y = win_h / 2 - r.h / 2 - m;
	SDL_RenderFillRect(renderer, &r);

	SDL_QueryTexture(title_t, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 2 - r.w / 2;
	r.y = 250 * flexible - m;
	SDL_RenderCopy(renderer, title_t, NULL, &r);
	
	SDL_QueryTexture(volume_text_t, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 2 - r.w / 2 - 280 * flexible;
	r.y = 400 * flexible - m;
	SDL_RenderCopy(renderer, volume_text_t, NULL, &r);

	SDL_QueryTexture(volume_t, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 2 - r.w / 2 + 300 * flexible;
	r.y = 400 * flexible - m;
	SDL_RenderCopy(renderer, volume_t, NULL, &r);

	SDL_Point p1, p2, p3, p4;
	p1 = { (int)(650 * flexible), (int)(409 * flexible) - m };
	p2 = { (int)(650 * flexible), (int)(409 * flexible + 40 * flexible) - m };
	p3 = { (int)(650 * flexible + 350 * flexible), (int)(409 * flexible + 40 * flexible) - m };
	p4 = { (int)(650 * flexible + 350 * flexible), (int)(409 * flexible) - m };
	DrawLine(p1, p2, renderer, 9);
	DrawLine(p2, p3, renderer, 9);
	DrawLine(p3, p4, renderer, 9);
	DrawLine(p4, p1, renderer, 9);

	SDL_Rect volume_bar;
	volume_bar.x = p1.x;
	volume_bar.y = p1.y;
	volume_bar.h = p3.y - p1.y;
	volume_bar.w = (p3.x - p1.x) * volume / 100;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &volume_bar);

	SDL_QueryTexture(offset_text_t, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 2 - r.w / 2 - 280 * flexible;
	r.y = 500 * flexible - m;
	SDL_RenderCopy(renderer, offset_text_t, NULL, &r);

	SDL_QueryTexture(offset_t, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 2 - r.w / 2 + 300 * flexible;
	r.y = 500 * flexible - m;
	SDL_RenderCopy(renderer, offset_t, NULL, &r);

	p1 = { (int)(650 * flexible), (int)(509 * flexible) - m };
	p2 = { (int)(650 * flexible), (int)(509 * flexible + 40 * flexible) - m };
	p3 = { (int)(650 * flexible + 350 * flexible), (int)(509 * flexible + 40 * flexible) - m };
	p4 = { (int)(650 * flexible + 350 * flexible), (int)(509 * flexible) - m };
	DrawLine(p1, p2, renderer, 9);
	DrawLine(p2, p3, renderer, 9);
	DrawLine(p3, p4, renderer, 9);
	DrawLine(p4, p1, renderer, 9);

	SDL_Rect offset_bar;
	offset_bar.x = p1.x;
	offset_bar.y = p1.y;
	offset_bar.h = p3.y - p1.y;
	offset_bar.w = (p3.x - p1.x) * (offset + 500) / 1000;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &offset_bar);

	close_btn->MoveButton({800, 650 - (int)(m / flexible)});
	close_btn->Rendering(renderer);

	if (close_effect_cnt > EFFECT_MAX) {
		editing = false;
	}
}

void Setting::Processing(SDL_Event * event, SDL_Renderer *renderer) {
	if (!close_effect_cnt && !open_effect_cnt) {
		if (close_btn->CheckClick(*event)) {
			close_effect_cnt = 1;
		}
		if (event->type == SDL_MOUSEBUTTONDOWN) {
			SDL_Point p1, p2;
			p1 = { (int)(650 * flexible), (int)(409 * flexible) };
			p2 = { (int)(650 * flexible + 350 * flexible), (int)(409 * flexible + 40 * flexible) };

			if (event->motion.y >= p1.y && event->motion.y <= p2.y && event->motion.x >= p1.x && event->motion.x <= p2.x) {
				int v = event->motion.x - p1.x;
				v = v * 100 / (p2.x - p1.x);
				ChangeVolume(v, renderer);
				volume_editing = true;
			}

			p1 = { (int)(650 * flexible), (int)(509 * flexible) };
			p2 = { (int)(650 * flexible + 350 * flexible), (int)(509 * flexible + 40 * flexible) };

			if (event->motion.y >= p1.y && event->motion.y <= p2.y && event->motion.x >= p1.x && event->motion.x <= p2.x) {
				int v = event->motion.x - p1.x;
				v = v * 1000 / (p2.x - p1.x) - 500;
				ChangeOffset(v, renderer);
				offset_editing = true;
			}
		}
		else if (event->type == SDL_MOUSEMOTION && volume_editing) {
			if (event->motion.state == SDL_PRESSED) {
				SDL_Point p1, p2;
				p1 = { (int)(650 * flexible), (int)(409 * flexible) };
				p2 = { (int)(650 * flexible + 350 * flexible), (int)(409 * flexible + 40 * flexible) };
				int v = event->motion.x - p1.x;
				if (v > p2.x - p1.x)
					v = p2.x - p1.x;
				if (v < 0)
					v = 0;
				v = v * 100 / (p2.x - p1.x);
				ChangeVolume(v, renderer);
			}
		}
		else if (event->type == SDL_MOUSEMOTION && offset_editing) {
			if (event->motion.state == SDL_PRESSED) {
				SDL_Point p1, p2;
				p1 = { (int)(650 * flexible), (int)(509 * flexible) };
				p2 = { (int)(650 * flexible + 350 * flexible), (int)(509 * flexible + 40 * flexible) };
				int v = event->motion.x - p1.x;
				if (v > p2.x - p1.x)
					v = p2.x - p1.x;
				if (v < 0)
					v = 0;
				v = v * 1000 / (p2.x - p1.x) - 500;
				ChangeOffset(v, renderer);
			}
		}
		else if (event->type == SDL_MOUSEBUTTONUP) {
			volume_editing = false;
			offset_editing = false;
		}
	}
}

bool Setting::CheckSettingEditing() {
	return editing;
}
