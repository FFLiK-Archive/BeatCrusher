#include "Block.h"
#include "LoadTexture.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Background.h"
#include "ScoreManager.h"
using namespace std;
#define d(a) ((double)a * 3.1415926 / 180)

extern int win_w, win_h;
extern SDL_Renderer *debug_ren;
extern double flexible;

#define Offset (70 + 100)

int crash_start_point;
int crash_end_point;

SDL_Point Block::mouse_pos = { 0,0 };
SDL_Point Block::prev_mouse_pos = { 0,0 };

Mix_Chunk* Block::crush_eff[3];


Block::Block(SDL_Point pos, int degree, Direction dir, bool auto_crush) {
	crash_start_point = Offset - 50;
	crash_end_point = Offset + 50;

	this->crushed = false;
	this->size = 10;
	this->pos = { win_w / 2, win_h / 2 };
	this->target_pos = pos;
	this->degree = degree;
	this->cut_begin.x = -1;
	this->cut_end.x = -1;
	this->destroyed = false;
	this->crushed_line = -1;
	this->destroy_level = 0;
	this->distance = 0;		
	this->dir = dir;
	this->scored = false;
	this->auto_crush = auto_crush;

	this->perfect_crash_point = false;
	this->time = clock();
}

Block::~Block() {
}

bool CrushedPosition(SDL_Point begin, SDL_Point end, SDL_Point mouse) {
	int addX, addY;
	int counter = 0;
	int dx = end.x - begin.x;
	int dy = end.y - begin.y;
	if (dx < 0) {
		addX = -1;
		dx = -dx;
	}
	else {
		addX = 1;
	}
	if (dy < 0) {
		addY = -1;
		dy = -dy;
	}
	else {
		addY = 1;
	}

	int x = begin.x;
	int y = begin.y;

	if (dx >= dy) {
		for (int i = 0; i < dx; i++) {
			x += addX;
			counter += dy;
			if (counter >= dx) {
				y += addY;
				counter -= dx;
			}
			if (mouse.x == x && mouse.y == y) {
				return true;
			}
		}
	}
	else {
		for (int i = 0; i < dy; i++) {
			y += addY;
			counter += dx;
			if (counter >= dy) {
				x += addX;
				counter -= dy;
			}
			if (mouse.x == x && mouse.y == y) {
				return true;
			}
		}
	}
}

bool Block::CheckCrushed(){
	if (!this->crushed) {
		if (this->auto_crush) {
			if (this->distance >= Offset) {
				this->crushed = true;
				destroy_level = 0;
				int s = 4;
				for (int pow = 0; pow < particle_size - 1; pow++) {
					s *= s;
				}
				this->force.resize(s);
				for (int i = 0; i < s; i++) {
					this->force[i].x = rand() % 10 - 5;
					this->force[i].y = rand() % 10 - 7;
				}
				for (int i = 0; i < spark_size; i++) {
					this->spark.push_back({ rand() % 1000 - 500, rand() % 1000 - 500 });
					this->particle.push_back(rand() % 3 - 1);
				}
			}
		}
		else {
			SDL_Point p[4];
			int c = cos(d(this->degree)) * this->size * flexible;
			int s = sin(d(this->degree)) * this->size * flexible;
			p[0].x = this->pos.x - c;
			p[0].y = this->pos.y - s;
			p[1].x = this->pos.x + s;
			p[1].y = this->pos.y - c;
			p[2].x = this->pos.x + c;
			p[2].y = this->pos.y + s;
			p[3].x = this->pos.x - s;
			p[3].y = this->pos.y + c;

			if (this->degree == 45) {
				p[0].x--;
				p[0].y--;
				p[2].x++;
				p[2].y++;
			}

			double a[4], b[4];
			double A, B;
			//y = ax + b
			for (int i = 0; i < 4; i++) {
				int i1 = i, i2 = i + 1;
				if (i2 == 4)
					i2 = 0;
				a[i] = (double)(p[i1].y - p[i2].y) / (double)(p[i1].x - p[i2].x);
				b[i] = -1 * a[i] * p[i1].x + p[i1].y;
				if (a[i] == 0)
					a[i] = 0;
				//DrawLine({ 0,(int)b[i] }, {1600, (int)(a[i] * 1600.0 + b[i])}, debug_ren, 15, 50, 50, 50);
			}

			bool not_move = false;
			if (prev_mouse_pos.x == mouse_pos.x)
				prev_mouse_pos.x++, not_move = true;

			if (prev_mouse_pos.y == mouse_pos.y) {
				prev_mouse_pos.y++;
			}
			else {
				not_move = false;
			}
			if (not_move)
				return false;

			// 교점 이용
			A = (double)(prev_mouse_pos.y - mouse_pos.y) / (double)(prev_mouse_pos.x - mouse_pos.x);
			B = -A * prev_mouse_pos.x + prev_mouse_pos.y;

			//DrawLine({ 0, (int)(0 * A + B) }, { win_w, (int)(win_w * A + B) }, debug_ren, 15, 100, 100, 100);

			double kx[4], ky[4]; //교점
			for (int i = 0; i < 4; i++) {
				kx[i] = (double)(b[i] - B) / (double)(A - a[i]);
				ky[i] = a[i] * kx[i] + b[i];
			}

			/*if (this->degree == 45) {
				kx[1] = (double)p[1].x;
				kx[3] = (double)p[3].x;
				ky[1] = A * (double)p[1].x + B;
				ky[3] = A * (double)p[3].x + B;
			}*/

			//cout << kx[0] << " " << kx[1] << " " << kx[2] << " " << kx[3] << endl;

			for (int i = 0; i < 4; i++) {
				int i1 = i, i2 = i + 1;
				if (i == this->crushed_line)
					continue;
				if (i2 == 4)
					i2 = 0;
				if ((p[i1].x >= kx[i] && p[i2].x <= kx[i]) || (p[i1].x <= kx[i] && p[i2].x >= kx[i])) {
					if ((p[i1].y >= ky[i] && p[i2].y <= ky[i]) || (p[i1].y <= ky[i] && p[i2].y >= ky[i])) {
						//DrawLine({ (int)kx[i], (int)ky[i] }, { (int)kx[i] + 1 , (int)ky[i] + 1 }, debug_ren, 51, 255, 0, 0);
						if ((prev_mouse_pos.x >= kx[i] && mouse_pos.x <= kx[i]) || (prev_mouse_pos.x <= kx[i] && mouse_pos.x >= kx[i])) {
							if ((prev_mouse_pos.y >= ky[i] && mouse_pos.y <= ky[i]) || (prev_mouse_pos.y <= ky[i] && mouse_pos.y >= ky[i])) {
								if (this->CheckDirection(prev_mouse_pos, p[0], p[1], p[2], p[3])) {
									this->crushed = true;
									destroy_level = 0;
									int s = 4;
									for (int pow = 0; pow < particle_size - 1; pow++) {
										s *= s;
									}
									this->force.resize(s);
									for (int i = 0; i < s; i++) {
										this->force[i].x = rand() % 10 - 5;
										this->force[i].y = rand() % 10 - 7;
									}
									for (int i = 0; i < spark_size; i++) {
										this->spark.push_back({ rand() % 1000 - 500, rand() % 1000 - 500 });
										this->particle.push_back(rand() % 3 - 1);
									}
								}
								break;
							}
						}
					}
				}
			}
		}
	}
	else
		return false;

	if (this->crushed) {
		Mix_PlayChannel(-1, Block::crush_eff[rand() % 3], 0);
	}
	return this->crushed;
}

void Block::ChangeSize(int add) {
	this->size += add;
}

void Block::ChangePos(SDL_Point pos) {
	this->pos = pos;
}

void Block::Rotate(int degree) {
	this->degree = degree;
	this->degree %= 90;
}

int Block::GetDegree(){
	return this->degree;
}

SDL_Point make_point(int x, int y) {
	SDL_Point p;
	p.x = x;
	p.y = y;
	return p;
}

#define MID(p1, p2) make_point((p1.x + p2.x)/2, (p1.y + p2.y)/2)

void Block::RenderBlock(SDL_Renderer *renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4, int color_mode, int alpha) {
	int s = this->size / 20 + 10;
	s *= flexible;

	SDL_Color c;

	switch (color_mode) {
	case 0:
		c = { (Uint8)Background::GetColor(-10).r, (Uint8)Background::GetColor(-10).g, (Uint8)Background::GetColor(-10).b, 255 };
		break;
	case 1:
		c = { (Uint8)Background::GetColor(50).r, (Uint8)Background::GetColor(50).g, (Uint8)Background::GetColor(50).b, 255 };
		break;
	case 2:
		c = { 235, 84, 84, 255 };
		break;
	}
	DrawLine(p1, p2, renderer, s, c.r, c.g, c.b, alpha);
	DrawLine(p2, p3, renderer, s, c.r, c.g, c.b, alpha);
	DrawLine(p3, p4, renderer, s, c.r, c.g, c.b, alpha);
	DrawLine(p4, p1, renderer, s, c.r, c.g, c.b, alpha);

	switch (this->dir) {
	case LEFT:
		DrawLine(p2, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p3, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;

	case RIGHT:
		DrawLine(p1, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p4, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;

	case UP:
		DrawLine(p3, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p4, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;

	case DOWN:
		DrawLine(p1, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p2, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;
		
	case LEFTUP:
		DrawLine(p3, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p4, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;

	case RIGHTUP:
		DrawLine(p1, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p4, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;

	case RIGHTDOWN:
		DrawLine(p1, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p2, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;

	case LEFTDOWN:
		DrawLine(p2, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		DrawLine(p3, this->pos, renderer, s, c.r, c.g, c.b, alpha);
		break;
	}
}

void Block::RenderBlockParticle(SDL_Renderer * renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4, int crushed_level, int lv, int v) {
	if (lv > 0) {
		this->RenderBlockParticle(renderer, p1, MID(p1, p2), MID(p1, p3), MID(p1, p4), crushed_level, lv - 1, v + lv * lv * 0);
		this->RenderBlockParticle(renderer, MID(p1, p2), p2, MID(p2, p3), MID(p1, p3), crushed_level, lv - 1, v + lv * lv * 1);
		this->RenderBlockParticle(renderer, MID(p1, p3), MID(p2, p3), p3, MID(p3, p4), crushed_level, lv - 1, v + lv * lv * 2);
		this->RenderBlockParticle(renderer, MID(p1, p4), MID(p1, p3), MID(p3, p4), p4, crushed_level, lv - 1, v + lv * lv * 3);
	}
	else {
		p1.x += this->force[v].x * crushed_level - this->particle[v] * 10;
		p2.x += this->force[v].x * crushed_level + this->particle[v] * 10;
		p3.x += this->force[v].x * crushed_level + this->particle[v] * 10;
		p4.x += this->force[v].x * crushed_level - this->particle[v] * 10;
		p1.y += this->force[v].y * crushed_level + crushed_level * crushed_level / 50 - this->particle[v] * 10;
		p2.y += this->force[v].y * crushed_level + crushed_level * crushed_level / 50 - this->particle[v] * 10;
		p3.y += this->force[v].y * crushed_level + crushed_level * crushed_level / 50 + this->particle[v] * 10;
		p4.y += this->force[v].y * crushed_level + crushed_level * crushed_level / 50 + this->particle[v] * 10;
		if (p1.y < win_h) {
			this->destroyed = false;
		}

		int s = this->size / 20 + 10;
		s *= flexible;
		DrawLine(p1, p2, renderer, s, Background::GetColor(50).r, Background::GetColor(50).g, Background::GetColor(50).b, 50);
		DrawLine(p2, p3, renderer, s, Background::GetColor(50).r, Background::GetColor(50).g, Background::GetColor(50).b, 50);
		DrawLine(p3, p4, renderer, s, Background::GetColor(50).r, Background::GetColor(50).g, Background::GetColor(50).b, 50);
		DrawLine(p4, p1, renderer, s, Background::GetColor(50).r, Background::GetColor(50).g, Background::GetColor(50).b, 50);
	}
}

bool Block::CheckDirection(SDL_Point pos, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4) {
	if (this->dir == MID)
		return true;
	if (this->dir % 2) {
		switch (this->dir) {
		case LEFTUP:
			if (pos.x >= p4.x && pos.x <= p3.x && pos.y >= p3.y && pos.y <= p4.y
				&& this->prev_mouse_pos.x > this->mouse_pos.x && this->prev_mouse_pos.y > this->mouse_pos.y)
				return true;
			break;
		case LEFTDOWN:
			if (pos.x >= p2.x && pos.x <= p3.x && pos.y >= p2.y && pos.y <= p3.y
				&& this->prev_mouse_pos.x > this->mouse_pos.x && this->prev_mouse_pos.y < this->mouse_pos.y)
				return true;
			break;
		case RIGHTUP:
			if (pos.x >= p1.x && pos.x <= p4.x && pos.y >= p1.y && pos.y <= p4.y
				&& this->prev_mouse_pos.x < this->mouse_pos.x && this->prev_mouse_pos.y > this->mouse_pos.y)
				return true;
			break;
		case RIGHTDOWN:
			if (pos.x >= p1.x && pos.x <= p2.x && pos.y >= p2.y && pos.y <= p1.y
				&& this->prev_mouse_pos.x < this->mouse_pos.x && this->prev_mouse_pos.y < this->mouse_pos.y)
				return true;
			break;
		}
	}
	else {
		switch (this->dir) {
		case RIGHT:
			if (prev_mouse_pos.x <= p1.x && mouse_pos.x >= p1.x)
				return true;
			break;
		case LEFT:
			if (prev_mouse_pos.x >= p3.x && mouse_pos.x <= p3.x)
				return true;
			break;
		case DOWN:
			if (prev_mouse_pos.y <= p1.y && mouse_pos.y >= p1.y)
				return true;
			break;
		case UP:
			if (prev_mouse_pos.y >= p3.y && mouse_pos.y <= p3.y)
				return true;
			break;
		}
	}
	return false;
}

void Block::ScoreCalc() {
	if (!this->scored) {
		this->scored = true;
		int s = abs(this->distance - Offset);
		if (this->distance >= crash_end_point) {
			TextEffect::PushEffect("MISS", this->pos);
			ScoreManager::AddResult("MISS");
			return;
		}
		s /= 15;
		switch (s) {
		case 0:
			TextEffect::PushEffect("PERFECT!!", this->pos);
			ScoreManager::AddResult("PERFECT!!");
			break;
		case 1:
			TextEffect::PushEffect("GREAT", this->pos);
			ScoreManager::AddResult("GREAT");
			break;
		case 2:
			TextEffect::PushEffect("GOOD", this->pos);
			ScoreManager::AddResult("GOOD");
			break;
		default:
			cout << s << " " << this->distance << crash_end_point << endl;
			TextEffect::PushEffect("OK", this->pos);
			ScoreManager::AddResult("OK");
			break;
		}
	}
}

void Block::Rendering(SDL_Renderer * renderer) {
	SDL_Point p1, p2, p3, p4;
	int c = cos(d(this->degree)) * this->size * flexible;
	int s = sin(d(this->degree)) * this->size * flexible;
	p1.x = this->pos.x - c;
	p1.y = this->pos.y - s;
	p2.x = this->pos.x + s;
	p2.y = this->pos.y - c;
	p3.x = this->pos.x + c;
	p3.y = this->pos.y + s;
	p4.x = this->pos.x - s;
	p4.y = this->pos.y + c;
	/*SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
	SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
	SDL_RenderDrawLine(renderer, p3.x, p3.y, p4.x, p4.y);
	SDL_RenderDrawLine(renderer, p4.x, p4.y, p1.x, p1.y);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);*/

	int ds = this->destroy_level * 2.5;

	if (this->crushed) {
		this->destroyed = true;
		this->RenderBlockParticle(renderer, p1, p2, p3, p4, ds);
		SDL_Point pos;
		if (ds < 500) {
			for (int i = 0; i < this->spark.size(); i++) {
				pos = this->pos;
				pos.x += this->spark[i].x * ds / 150;
				pos.y += this->spark[i].y * ds / 100 + ds * ds / 100;
				int alpha = (500 - ds) * 255 / 500;
				DrawLine(pos, { pos.x + 1, pos.y + 1 }, renderer, rand() % 30 + 10, 255, 255, 255, alpha);
			}
		}
	}
	else {
		if (this->distance <= crash_start_point) {
			this->RenderBlock(renderer, p1, p2, p3, p4, 0, 255);
		}
		else if(this->distance >= crash_end_point) {
			this->RenderBlock(renderer, p1, p2, p3, p4, 2, 255 - ds * 2);
		}
		else {
			this->RenderBlock(renderer, p1, p2, p3, p4, 1, 255);
		}
	}
}

bool Block::Processing(bool check, bool process) {
	if (this->crushed) {
		return false;
	}

	if (this->distance >= crash_end_point) {
		return false;
	}

	if (prev_mouse_pos.x == mouse_pos.x && prev_mouse_pos.y == mouse_pos.y || this->distance <= crash_start_point)
		return false;

	if (!check)
		return false;

	return this->CheckCrushed();
}

void Block::Move() {
	if (!this->crushed) {
		this->distance = (clock() - this->time) / 5;
		this->size += 0.004 * (double)this->distance;
		this->pos.x = win_w / 2 + (this->target_pos.x - win_w / 2) * this->distance / block_target_point;
		this->pos.y = win_h / 2 + (this->target_pos.y - win_h / 2) * this->distance / block_target_point;
	}
	else {
		ScoreCalc();
		this->destroy_level++;
		if (this->destroy_level < 60) {
			Background::ChangeAlpha(30 - abs(this->destroy_level - 30) + 5);
		}
	}

	if(this->distance >= crash_end_point) {
		ScoreCalc();
		this->destroy_level++;
		if (this->destroy_level * 2.5 * 2 > 255) {
			this->destroyed = true;
		}
	}
}

bool Block::Able() {
	if (!this->crushed && this->distance <= crash_end_point)
		return true;
	return false;
}

void Block::Delay(int delay) {
	this->time += delay;
}

vector<string> TextEffect::str;
vector<int> TextEffect::effect;
vector<SDL_Texture*> TextEffect::tex;
vector<SDL_Point> TextEffect::pos;

void TextEffect::Init() {
	str.clear();
	effect.clear();
	pos.clear();
	for (int i = 0; i < tex.size(); i++) {
		SDL_DestroyTexture(tex[i]);
		tex[i] = nullptr;
	}
	tex.clear();
}

void TextEffect::PushEffect(string s, SDL_Point p) {
	str.push_back(s);
	effect.push_back(0);
	tex.push_back(nullptr);
	pos.push_back(p);
}

void TextEffect::Rendering(SDL_Renderer * renderer) {
	for (int i = 0; i < effect.size(); i++) {
		if (!tex[i]) {
			if (str[i] == "PERFECT!!") {
				tex[i] = LoadText(str[i].c_str(), renderer, 30 * flexible, "main_font", 190, 255, 176); //#beffb0
			}
			else if (str[i] == "GREAT") {
				tex[i] = LoadText(str[i].c_str(), renderer, 30 * flexible, "main_font", 217, 194, 255); //#d9c2ff
			}
			else if (str[i] == "GOOD") {
				tex[i] = LoadText(str[i].c_str(), renderer, 30 * flexible, "main_font", 176, 241, 255); //#b0f1ff
			}
			else if (str[i] == "OK") {
				tex[i] = LoadText(str[i].c_str(), renderer, 30 * flexible, "main_font", 232, 232, 232); //#e8e8e8
			}
			else {
				tex[i] = LoadText(str[i].c_str(), renderer, 30 * flexible, "main_font", 255, 204, 204); //#ffcccc
			}
		}
		SDL_Rect r;
		SDL_QueryTexture(tex[i], NULL, NULL, &r.w, &r.h);
		r.x = pos[i].x - r.w / 2;
		r.y = pos[i].y - r.h / 2;
		SDL_SetTextureAlphaMod(tex[i], 255 - effect[i]);
		SDL_RenderCopy(renderer, tex[i], NULL, &r);
		
	}
}

void TextEffect::Processing() {
	for (int i = 0; i < effect.size(); i++) {
		effect[i] += 2;
		if (effect[i] >= 255) {
			effect.erase(effect.begin() + i);
			pos.erase(pos.begin() + i);
			str.erase(str.begin() + i);
			SDL_DestroyTexture(tex[i]);
			tex.erase(tex.begin() + i);
		}
		pos[i].y -= ((double)(255 - effect[i]) / 400.0) * flexible;
	}
}
