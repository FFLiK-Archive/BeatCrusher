#pragma once
#include <SDL.h>
#include <string>
using namespace std;
struct Result {
	int perfect;
	int great;
	int good;
	int ok;
	int miss;
};

class ScoreManager {
private:
	static int score;
	static Result result;
	static int combo;
	static int combo_score;

	static SDL_Texture *score_t, *combo_t, *score_text, *combo_text, *block_t, *block_text;
	static int change_score;

	static int total_blocks;
	static int crushed_blocks;

public:
	static void Init(SDL_Renderer *renderer, int blocks);
	static void Destroy();
	static void Rendering(SDL_Renderer *renderer);
	static void AddResult(string s);

	static Result GetResult();
	static int GetScore();
	static int GetBlocks();
};

