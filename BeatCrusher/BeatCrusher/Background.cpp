#include "Background.h"
#include "LoadTexture.h"
#include "ColorCalc.h"
#include <iostream>
#include "SetDataFile.h"
using namespace std;
using namespace Color;
#define d(a) ((double)a * 3.1415926 / 180.0)
#define CHANGE_LEVEL 50

extern int win_w, win_h;
extern double flexible;

RGB_Color Background::current_color, Background::prev_color;
SDL_Texture* Background::game_background_tex;
SDL_Rect Background::background;
vector<double> Background::game_effect_degree;
vector<int> Background::color_change_pos;
int Background::alpha, Background::change_level;
vector<Color::RGB_Color> Background::color_table;

void Background::Init(SDL_Renderer * renderer, string folder_name) {
	change_level = 0;
	game_background_tex = LoadImage(Location(RES, "", "background.img").c_str(), renderer);
	if (win_w / 16 > win_h / 9) {
		background.w = win_w;
		background.h = win_w / 16 * 9;
		background.x = 0;
		background.y = -(background.h - win_h)/2;
	}
	else {
		background.h = win_h;
		background.w = win_h / 9 * 16;
		background.y = 0;
		background.x = -(background.w - win_w) / 2;
	}
	for (int i = 0; i < 3; i++) {
		game_effect_degree.push_back(i * 10);
	}

	SDL_RWops *f;
	f = SDL_RWFromFile(Location(DAT, folder_name, "clr.dat").c_str(), "r");
	int fsize = GetSize(Location(DAT, folder_name, "clr.dat"));
	while (SDL_RWseek(f, 0, RW_SEEK_CUR) != fsize) {
		int p;
		int r, g, b;
		p = ReadNextInt(f, fsize);
		r = ReadNextInt(f, fsize);
		g = ReadNextInt(f, fsize);
		b = ReadNextInt(f, fsize);
		color_change_pos.push_back(p);
		color_table.push_back({ r,g,b });
	}
	SDL_RWclose(f);

	current_color = color_table[0];
	SDL_SetTextureColorMod(game_background_tex, color_table[0].r, color_table[0].g, color_table[0].b);
}

void Background::NewBackground(string folder_name) {
	color_change_pos.clear();
	color_table.clear();

	SDL_RWops *f;
	f = SDL_RWFromFile(Location(DAT, folder_name, "clr.dat").c_str(), "r");
	int fsize = GetSize(Location(DAT, folder_name, "clr.dat"));
	while (SDL_RWseek(f, 0, RW_SEEK_CUR) != fsize) {
		int p;
		int r, g, b;
		p = ReadNextInt(f, fsize);
		r = ReadNextInt(f, fsize);
		g = ReadNextInt(f, fsize);
		b = ReadNextInt(f, fsize);
		color_change_pos.push_back(p);
		color_table.push_back({ r,g,b });
	}
	SDL_RWclose(f);

	prev_color = current_color;
	change_level = CHANGE_LEVEL;
}

void Background::Destroy() {
	game_effect_degree.clear();
	SDL_DestroyTexture(game_background_tex);
}

void Background::Processing() {
	if (change_level) {
		change_level--;
	}

	for (int i = 0; i < game_effect_degree.size(); i++) {
		game_effect_degree[i] += ((double)alpha * 0.02 * (i + 1));
		if (game_effect_degree[i] >= 90)
			game_effect_degree[i] -= 90;
	}
}

void Background::Rendering(SDL_Renderer * renderer) {

	if (change_level) {
		RGB_Color c, from, to;
		from = prev_color;
		to = color_table[0];
		
		c.r = (from.r * (change_level - 1) + to.r * (CHANGE_LEVEL - (change_level - 1))) / CHANGE_LEVEL;
		c.g = (from.g * (change_level - 1) + to.g * (CHANGE_LEVEL - (change_level - 1))) / CHANGE_LEVEL;
		c.b = (from.b * (change_level - 1) + to.b * (CHANGE_LEVEL - (change_level - 1))) / CHANGE_LEVEL;

		current_color = c;

		if (change_level == 1) {
			current_color = to;
		}

		SDL_SetTextureColorMod(game_background_tex, current_color.r, current_color.g, current_color.b);
	}

	if (win_w / 16 > win_h / 9) {
		background.w = win_w;
		background.h = win_w / 16 * 9;
		background.x = 0;
		background.y = -(background.h - win_h) / 2;
	}
	else {
		background.h = win_h;
		background.w = win_h / 9 * 16;
		background.y = 0;
		background.x = -(background.w - win_w) / 2;
	}

	background.w += (alpha - 5) * 2;
	background.h += (alpha - 5) * 2;
	background.x -= (alpha - 5) * 1;
	background.y -= (alpha - 5) * 1;

	SDL_RenderCopy(renderer, game_background_tex, NULL, &background);

	SDL_Point p1, p2, p3, p4;
	int c, s;
	for (int i = 0; i < game_effect_degree.size(); i++) {
		c = cos(d(game_effect_degree[i])) * (i * i * 200 + 100) * flexible;
		s = sin(d(game_effect_degree[i])) * (i * i * 200 + 100) * flexible;
		p1.x = win_w / 2 - c;
		p1.y = win_h / 2 - s;
		p2.x = win_w / 2 + s;
		p2.y = win_h / 2 - c;
		p3.x = win_w / 2 + c;
		p3.y = win_h / 2 + s;
		p4.x = win_w / 2 - s;
		p4.y = win_h / 2 + c;

		int t = 50 + i * 20;

		DrawLine(p1, p2, renderer, t, current_color.r, current_color.g, current_color.b, alpha);
		DrawLine(p2, p3, renderer, t, current_color.r, current_color.g, current_color.b, alpha);
		DrawLine(p3, p4, renderer, t, current_color.r, current_color.g, current_color.b, alpha);
		DrawLine(p4, p1, renderer, t, current_color.r, current_color.g, current_color.b, alpha);
	}

	/*if (change_level) {
		SDL_Rect white;
		white.x = 0;
		white.y = 0;
		white.w = win_w;
		white.h = win_h;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, change_level * 255 / CHANGE_LEVEL);
		SDL_RenderFillRect(renderer, &white);
	}*/
}

void Background::ChangeColor(int crashed_block_num) {
	if (color_change_pos.size() > 1 && crashed_block_num == color_change_pos[1]) {
		color_change_pos.erase(color_change_pos.begin());
		color_table.erase(color_table.begin());
		prev_color = current_color;
		change_level = CHANGE_LEVEL;
	}
}

void Background::ChangeColorRGB(Color::RGB_Color color) {
	color_table[0] = color;
	prev_color = current_color;
	change_level = CHANGE_LEVEL;
}

void Background::ChangeAlpha(int a) {
	Background::alpha = a;
}

RGB_Color Background::GetColor(int bright_level) {
	if(!bright_level)
		return current_color;
	else {
		HSV_Color hsv;
		hsv = RGBtoHSV(current_color);
		hsv.v += bright_level;
		hsv.s -= bright_level / 2;
		if (hsv.v > 100)
			hsv.v = 100;
		if (hsv.v < 0)
			hsv.v = 0;
		if (hsv.s > 100)
			hsv.s = 100;
		if (hsv.s < 0)
			hsv.s = 0;
		return HSVtoRGB(hsv);
	}
}
