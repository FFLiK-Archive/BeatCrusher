#include "TimeEditer.h"
#include "LoadTexture.h"
#include <iostream>
using namespace std;

TimeEditer::TimeEditer(SDL_Renderer * renderer) {
	this->time_btn.MakeButton(renderer, "time", 1000, 300, 100, 50, 25, 50, 50, 50, 200, 200, 200);
	this->loop_btn.MakeButton(renderer, "loop", 1000, 380, 100, 50, 25, 50, 50, 50, 200, 200, 200);
	this->apply_btn.MakeButton(renderer, "apply", 1480, 490, 100, 50, 25, 50, 50, 50, 200, 200, 200);
	this->SetLoop(0, renderer);
	this->SetTime(0, renderer);
	this->time_change = true;
	this->time_btn.Select(false);
	this->loop_btn.Select(true);
	this->time_pos = 0;
	this->dot = false;
}

TimeEditer::~TimeEditer() {
	SDL_DestroyTexture(this->time_t);
	SDL_DestroyTexture(this->loop_t);
	this->apply_btn.Destroy();
	this->loop_btn.Destroy();
	this->time_btn.Destroy();
}

double TimeEditer::GetTime() {
	return (double)this->time_i + stod("0." + this->time_d);
}

int TimeEditer::GetLoop() {
	return this->loop;
}

int TimeEditer::SetTime(double t, SDL_Renderer *renderer) {
	if (t != -1) {
		string s = to_string(t);
		this->dot = false;
		int d = 0;
		this->time_pos = 0;
		for (int i = 0; i < s.size(); i++) {
			if (s[i] == '.') {
				d = i;
				continue;
			}	
			if (d && s[i] != '0') {
				this->dot = true;
				this->time_pos = i - d;
			}	
		}
		
		this->time_i = t;
		this->time_d = to_string(t);
		while (this->time_d.front() != '.')
			this->time_d.erase(time_d.begin());
		this->time_d.erase(time_d.begin());
	}
		
	if (this->time_t)
		SDL_DestroyTexture(this->time_t);
	string s = (to_string(this->time_i) + "." + this->time_d);
	this->time_t = LoadText(s.c_str(), renderer, 30, "MainFont");
	return 0;
}

int TimeEditer::SetLoop(int l, SDL_Renderer * renderer) {
	if(l != -1)
		this->loop = l;
	if (this->loop_t)
		SDL_DestroyTexture(this->loop_t);
	this->loop_t = LoadText(to_string(this->loop).c_str(), renderer, 30, "MainFont");
	return 0;
}

bool TimeEditer::Event(SDL_Event event, SDL_Renderer *renderer) {

	if (this->time_btn.Click(event)) {
		this->time_change = true;
		this->time_btn.Select(false);
		this->loop_btn.Select(true);
	}
	else if(this->loop_btn.Click(event)) {
		this->time_change = false;
		this->time_btn.Select(true);
		this->loop_btn.Select(false);
	}

	if (event.type == SDL_KEYUP) {
		char c = (char)event.key.keysym.sym;
		if (c == SDLK_TAB) {
			if (this->time_change) {
				this->loop_btn.Select(false);
				this->time_btn.Select(true);
				this->time_change = false;
			}
			else {
				this->loop_btn.Select(true);
				this->time_btn.Select(false);
				this->time_change = true;
			}
		}
		if (this->time_change) {
			if (c >= '0' && c <= '9') {
				if (!this->dot) {
					string t = to_string(this->time_i);
					if (t.size() > 6)
						return false;
					this->time_i = stoi(t + c);
					this->SetTime(-1, renderer);
				}
				else {
					string t = this->time_d;
					if (this->time_pos > 5)
						return false;
					t[this->time_pos] = c;
					this->time_pos++;
					this->time_d = t;
					this->SetTime(-1, renderer);
				}
			}
			else if (c == '.') {
				this->dot = true;
			}
			else if (c == SDLK_BACKSPACE) {
				if (!this->dot) {
					ERASE:
					string t = to_string(this->time_i);
					t.pop_back();
					if (t.empty())
						t = "0";
					this->time_i = stoi(t);
					this->SetTime(-1, renderer);
				}
				else {
					string t = this->time_d;
					this->time_pos--;
					if (this->time_pos < 0) {
						this->dot = false;
						this->time_pos = 0;
						goto ERASE;
					}
					t[this->time_pos] = '0';
					this->time_d = t;
					this->SetTime(-1, renderer);
				}
			}
		}
		else {
			if (c >= '0' && c <= '9') {
				string t = to_string(this->loop);
				if (t.size() > 6)
					return false;
				this->loop = stoi(t + c);
				this->SetLoop(-1, renderer);
			}
			else if (c == SDLK_BACKSPACE) {
				string t = to_string(this->loop);
				t.pop_back();
				if (t.empty())
					t = "0";
				this->loop = stoi(t);
				this->SetLoop(-1, renderer);
			}
		}
	}

	return this->apply_btn.Click(event) || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_RETURN);
}

void TimeEditer::Rendering(SDL_Renderer * renderer) {
	this->apply_btn.Rendering();
	this->loop_btn.Rendering();
	this->time_btn.Rendering();
	SDL_Rect r;

	SDL_QueryTexture(this->time_t, NULL, NULL, &r.w, &r.h);
	r.x = 1200;
	r.y = 305;
	SDL_RenderCopy(renderer, this->time_t, NULL, &r);

	SDL_QueryTexture(this->loop_t, NULL, NULL, &r.w, &r.h);
	r.x = 1200;
	r.y = 385;
	SDL_RenderCopy(renderer, this->loop_t, NULL, &r);

	r.x = 990;
	r.y = 290;
	r.w = 590;
	r.h = 70;
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderDrawRect(renderer, &r);
	r.x = 990;
	r.y = 370;
	r.w = 590;
	r.h = 70;
	SDL_RenderDrawRect(renderer, &r);
}
