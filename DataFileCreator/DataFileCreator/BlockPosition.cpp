#include "BlockPosition.h"
#include "LoadTexture.h"
#include <iostream>
using namespace std;

SDL_Texture* BlockPosition::arrow[9];

BlockPosition::BlockPosition(SDL_Renderer * renderer) {
	this->left.MakeButton(renderer, "¡ì", 850, 20, 50, 50, 30, 50, 50, 50, 150, 150, 150);
	this->right.MakeButton(renderer, "¡í", 910, 20, 50, 50, 30, 50, 50, 50, 150, 150, 150);
	this->show_page = 0;
	this->page_tex = LoadText("0 / 0", renderer, 30, "MainFont", 200, 200, 200);
	this->pos.clear();
	this->dir.clear();
}

BlockPosition::~BlockPosition() {
	this->left.Destroy();
	this->right.Destroy();
}

void BlockPosition::Rendering(SDL_Renderer * renderer, bool playing, int alpha) {
	if (playing) {
		if (this->pos[this->show_page].x && this->pos[this->show_page].y) {
			SDL_Rect r;
			r.x = 180;
			r.y = 100;
			r.w = 120;
			r.h = 120;
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);

			r.x = r.x + (this->pos[this->show_page].x - 1) * r.w + 10;
			r.y = r.y + (this->pos[this->show_page].y - 1) * r.h + 10;
			r.w = r.w - 20;
			r.h = r.h - 20;

			SDL_RenderFillRect(renderer, &r);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
			r.x = 1300;
			r.y = 30;
			r.w = 70;
			r.h = 70;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					SDL_Rect d;
					d.x = r.x + i * r.w + 5;
					d.y = r.y + j * r.h + 5;
					d.w = r.w - 10;
					d.h = r.h - 10;
					SDL_RenderDrawRect(renderer, &d);
					if (i == this->dir[this->show_page].x - 1 && j == this->dir[this->show_page].y - 1) {
						SDL_RenderFillRect(renderer, &d);
						SDL_Rect ar = r;
						ar.x -= 200;
						ar.y += 50;
						SDL_QueryTexture(arrow[i * 3 + j], NULL, NULL, &ar.w, &ar.h);
						SDL_RenderCopy(renderer, arrow[i * 3 + j], NULL, &ar);
					}
				}
			}
		}
	}
	else {
		if (!this->pos.empty()) {
			this->left.Select(true);
			this->right.Select(true);
			if (this->show_page == 0) {
				this->left.Select(false);
			}
			if (this->show_page >= this->pos.size() - 1) {
				this->right.Select(false);
			}

			SDL_Rect r;
			r.x = 180;
			r.y = 100;
			r.w = 120;
			r.h = 120;
			SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 3; j++) {
					SDL_Rect d;
					d.x = r.x + i * r.w + 10;
					d.y = r.y + j * r.h + 10;
					d.w = r.w - 20;
					d.h = r.h - 20;
					SDL_RenderDrawRect(renderer, &d);
					if (i == this->pos[this->show_page].x - 1 && j == this->pos[this->show_page].y - 1) {
						SDL_RenderFillRect(renderer, &d);
					}
				}
			}

			r.x = 1300;
			r.y = 30;
			r.w = 70;
			r.h = 70;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					SDL_Rect d;
					d.x = r.x + i * r.w + 5;
					d.y = r.y + j * r.h + 5;
					d.w = r.w - 10;
					d.h = r.h - 10;
					SDL_RenderDrawRect(renderer, &d);
					if (i == this->dir[this->show_page].x - 1 && j == this->dir[this->show_page].y - 1) {
						SDL_RenderFillRect(renderer, &d);
						SDL_Rect ar = r;
						ar.x -= 200;
						ar.y += 50;
						SDL_QueryTexture(arrow[i * 3 + j], NULL, NULL, &ar.w, &ar.h);
						SDL_RenderCopy(renderer, arrow[i * 3 + j], NULL, &ar);
					}
				}
			}
		}
		else {
			this->left.Select(false);
			this->right.Select(false);

			SDL_Rect r;
			r.x = 180;
			r.y = 100;
			r.w = 120;
			r.h = 120;
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 3; j++) {
					SDL_Rect d;
					d.x = r.x + i * r.w + 10;
					d.y = r.y + j * r.h + 10;
					d.w = r.w - 20;
					d.h = r.h - 20;
					SDL_RenderDrawRect(renderer, &d);
				}
			}

			r.x = 1000;
			r.y = 30;
			r.w = 60;
			r.h = 60;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					SDL_Rect d;
					d.x = r.x + i * r.w + 5;
					d.y = r.y + j * r.h + 5;
					d.w = r.w - 10;
					d.h = r.h - 10;
					SDL_RenderDrawRect(renderer, &d);
				}
			}
		}
		SDL_Rect r;
		r.x = 20;
		r.y = 20;
		SDL_QueryTexture(this->page_tex, NULL, NULL, &r.w, &r.h);
		SDL_RenderCopy(renderer, this->page_tex, NULL, &r);
		this->left.Rendering();
		this->right.Rendering();
	}
}

void BlockPosition::Event(SDL_Event event, SDL_Renderer *renderer) {
	if (this->left.Click(event)) {
		this->right.Select(true);
		this->show_page--;
		if (this->page_tex)
			SDL_DestroyTexture(this->page_tex);
		this->page_tex = LoadText((to_string(this->show_page + 1) + " / " + to_string(this->pos.size())).c_str(), renderer, 30, "MainFont", 200, 200, 200);
		if (this->show_page == 0) {
			this->left.Select(false);
		}
	}
	else if (this->right.Click(event)) {
		this->left.Select(true);
		this->show_page++;
		if (this->page_tex)
			SDL_DestroyTexture(this->page_tex);
		this->page_tex = LoadText((to_string(this->show_page + 1) + " / " + to_string(this->pos.size())).c_str(), renderer, 30, "MainFont", 200, 200, 200);
		if (this->show_page >= this->pos.size() - 1) {
			this->right.Select(false);
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP) {
		int x = event.button.x;
		int y = event.button.y;
		SDL_Rect r;
		r.x = 180;
		r.y = 100;
		r.w = 120;
		r.h = 120;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 3; j++) {
				SDL_Rect d;
				d.x = r.x + i * r.w + 10;
				d.y = r.y + j * r.h + 10;
				d.w = r.w - 20;
				d.h = r.h - 20;
				if (x >= d.x && x <= d.x + d.w && y >= d.y && y <= d.y + d.h) {
					this->pos[this->show_page].x = i + 1;
					this->pos[this->show_page].y = j + 1;
				}
			}
		}

		r.x = 1300;
		r.y = 30;
		r.w = 70;
		r.h = 70;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				SDL_Rect d;
				d.x = r.x + i * r.w + 5;
				d.y = r.y + j * r.h + 5;
				d.w = r.w - 10;
				d.h = r.h - 10;
				if (x >= d.x && x <= d.x + d.w && y >= d.y && y <= d.y + d.h) {
					this->dir[this->show_page].x = i + 1;
					this->dir[this->show_page].y = j + 1;
				}
			}
		}
	}
}

void BlockPosition::SetSize(int size, SDL_Renderer *renderer) {
	int prev_size = this->pos.size();
	this->pos.resize(size);
	this->dir.resize(size);
	for (int i = prev_size; i < size; i++) {
		this->pos[i] = { 0,0 };
		this->dir[i] = { 0,0 };
	}
	if (this->show_page >= size && size)
		this->show_page = size - 1;
	if (this->page_tex)
		SDL_DestroyTexture(this->page_tex);
	this->page_tex = LoadText((to_string(this->show_page + 1) + " / " + to_string(this->pos.size())).c_str(), renderer, 30, "MainFont", 200, 200, 200);
}

void BlockPosition::ChangePage(SDL_Renderer *renderer, int n) {
	if (this->show_page < this->pos.size() - 1) {
		this->show_page++;
	}
	else {
		this->show_page = 0;
	}
	
	if (n != -1)
		this->show_page = n;
	
	if (this->page_tex)
		SDL_DestroyTexture(this->page_tex);
	this->page_tex = LoadText((to_string(this->show_page + 1) + " / " + to_string(this->pos.size())).c_str(), renderer, 30, "MainFont", 200, 200, 200);
}

void BlockPosition::SavePos(FILE * f) {
	for (int i = 0; i < this->pos.size(); i++) {
		fprintf_s(f, "\n");
		fprintf_s(f, "%d %d", this->pos[i].x, this->pos[i].y);			
	}
}

void BlockPosition::LoadPos(FILE * f) {
	for (int i = 0; i < this->pos.size() && !feof(f); i++) {
		fscanf_s(f, "%d", &this->pos[i].x);
		fscanf_s(f, "%d", &this->pos[i].y);
	}
}

void BlockPosition::SaveDir(FILE * f) {
	for (int i = 0; i < this->dir.size(); i++) {
		fprintf_s(f, "\n");
		fprintf_s(f, "%d %d", this->dir[i].x, this->dir[i].y);
	}
}

void BlockPosition::LoadDir(FILE * f) {
	for (int i = 0; i < this->dir.size() && !feof(f); i++) {
		fscanf_s(f, "%d", &this->dir[i].x);
		fscanf_s(f, "%d", &this->dir[i].y);
	}
}

void BlockPosition::Init(SDL_Renderer * renderer) {
	int size = 100;
	arrow[0] = LoadText("¢Ø", renderer, size, "MainFont", 200, 200, 200);
	arrow[1] = LoadText("¡ç", renderer, size, "MainFont", 200, 200, 200);
	arrow[2] = LoadText("¢×", renderer, size, "MainFont", 200, 200, 200);
	arrow[3] = LoadText("¡è", renderer, size, "MainFont", 200, 200, 200);
	arrow[4] = LoadText("¢Á", renderer, size, "MainFont", 200, 200, 200);
	arrow[5] = LoadText("¡é", renderer, size, "MainFont", 200, 200, 200);
	arrow[6] = LoadText("¢Ö", renderer, size, "MainFont", 200, 200, 200);
	arrow[7] = LoadText("¡æ", renderer, size, "MainFont", 200, 200, 200);
	arrow[8] = LoadText("¢Ù", renderer, size, "MainFont", 200, 200, 200);
}

void BlockPosition::Destroy() {
	for (int i = 0; i < 9; i++) {
		SDL_DestroyTexture(arrow[i]);
		arrow[i] = nullptr;
	}
}
