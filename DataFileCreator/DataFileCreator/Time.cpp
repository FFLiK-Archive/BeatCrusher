#include "Time.h"
#include "LoadTexture.h"

Time::Time(int pos, SDL_Renderer *renderer) {
	this->r.x = 220;
	this->r.y = 570;
	this->r.w = 150;
	this->r.h = 190;
	this->block_pos = new BlockPosition(renderer);
	this->ChangeLoop(0, renderer);
	this->ChangeTime(0, renderer);
	this->ChangeNum(pos, renderer);
}

Time::~Time() {
	SDL_DestroyTexture(this->loop_t);
	SDL_DestroyTexture(this->time_t);
}

void Time::ChangeTime(double t, SDL_Renderer *renderer) {
	if(this->time_t)
		SDL_DestroyTexture(this->time_t);
	string s = to_string(t);
	while (s.back() == '0')
		s.pop_back();
	if (s.back() == '.')
		s.pop_back();
	int cnt = 0;
	bool dot = false;
	for (int i = 0; i < s.size(); i++) {
		if (dot)
			cnt++;
		if (s[i] == '.')
			dot = true;
	}
	cnt -= 3;
	for (int i = 0; i < cnt; i++)
		s.pop_back();
	this->time_t = LoadText(s.c_str(), renderer, 30, "MainFont");
	this->time = t;
}

void Time::ChangeLoop(int l, SDL_Renderer * renderer) {
	if(this->loop_t)
		SDL_DestroyTexture(this->loop_t);
	this->loop_t = LoadText(to_string(l).c_str(), renderer, 30, "MainFont");
	this->loop = l;
	this->block_pos->SetSize(l, renderer);
}

void Time::ChangeNum(int n, SDL_Renderer * renderer) {
	if(this->num_t)
		SDL_DestroyTexture(this->num_t);
	this->num_t = LoadText(to_string(n).c_str(), renderer, 30, "MainFont", 150, 150, 150);
	this->pos= n;
}

bool Time::Event(SDL_Event event) {
	SDL_Rect re;
	re = this->r;
	re.x += (re.w + 10) * this->pos;
	if (event.type == SDL_MOUSEBUTTONUP) {
		if (event.button.x <= re.x + re.w && event.button.x >= re.x) {
			if (event.button.y <= re.y + re.h && event.button.y >= re.y) {
				return true;
			}
		}
	}
	return false;
}

void Time::Rendering(SDL_Renderer * renderer, bool selected) {
	SDL_Rect t, l, n, re;
	re = this->r;
	re.x += (re.w + 10) * this->pos;
	SDL_QueryTexture(this->time_t, NULL, NULL, &t.w, &t.h);
	SDL_QueryTexture(this->loop_t, NULL, NULL, &l.w, &l.h);
	SDL_QueryTexture(this->num_t, NULL, NULL, &n.w, &n.h);
	
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
	if(selected)
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderFillRect(renderer, &re);
	t.x = re.x + re.w / 2 - t.w / 2;
	t.y = re.y + 20;
	l.x = re.x + re.w / 2 - l.w / 2;
	l.y = t.y + t.h + 20;
	n.x = re.x + re.w / 2 - n.w / 2;
	n.y = l.y + l.h + 20;
	SDL_RenderCopy(renderer, this->time_t, NULL, &t);
	SDL_RenderCopy(renderer, this->loop_t, NULL, &l);
	SDL_RenderCopy(renderer, this->num_t, NULL, &n);
}
