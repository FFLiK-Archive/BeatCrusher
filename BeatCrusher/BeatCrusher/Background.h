#pragma once
#include <SDL.h>
#include <vector>
#include <string>
#include "ColorCalc.h"
using namespace std;
class Background {
private:
	static SDL_Texture *game_background_tex;
	static SDL_Rect background;
	static vector<double> game_effect_degree;
	static Color::RGB_Color current_color, prev_color;
	static vector<int> color_change_pos;
	static vector<Color::RGB_Color> color_table;
	static int change_level;
	static int alpha;

public:
	
	static void Init(SDL_Renderer *renderer, string folder_name);
	static void NewBackground(string folder_name);
	static void Destroy();
	static void Processing();
	static void Rendering(SDL_Renderer *renderer);
	static void ChangeColor(int crashed_block_num);
	static void ChangeColorRGB(Color::RGB_Color color);
	static void ChangeAlpha(int a);
	static Color::RGB_Color GetColor(int bright_level = 0);
};

