#include "ScoreManager.h"
#include "LoadTexture.h"
#include "Background.h"
#include <iostream>
using namespace std;

#define SCORE_CHANGE_SIZE 10

#define MAX_SCORE 9000000
#define MAX_COMBO 1000000

extern int win_w, win_h;

int ScoreManager::score;
Result ScoreManager::result;
int ScoreManager::combo, ScoreManager::combo_score;

SDL_Texture* ScoreManager::score_t, *ScoreManager::combo_t, *ScoreManager::score_text, *ScoreManager::combo_text, *ScoreManager::block_t, *ScoreManager::block_text;
int ScoreManager::change_score;

int ScoreManager::total_blocks;
int ScoreManager::crushed_blocks;
extern double flexible;

void ScoreManager::Init(SDL_Renderer * renderer, int blocks) {
	score_t = LoadText("0", renderer, 70 * flexible, "main_font", 255, 255, 255);
	combo_t = LoadText("0", renderer, 50 * flexible, "main_font", 255, 255, 255);
	score_text = LoadText("SCORE", renderer, 20 * flexible, "main_font", 255, 255, 255);
	combo_text = LoadText("COMBO", renderer, 20 * flexible, "main_font", 255, 255, 255);
	block_text = LoadText("BLOCKS", renderer, 20 * flexible, "main_font", 255, 255, 255);
	block_t = LoadText(("0/" + to_string(blocks)).c_str(), renderer, 50 * flexible, "main_font", 255, 255, 255);

	total_blocks = blocks;
	crushed_blocks = 0;
	result.perfect = 0;
	result.great = 0;
	result.good = 0;
	result.ok = 0;
	result.miss = 0;
	score = 0;
	combo = 0;
	change_score = 0;
	combo_score = 0;
}

void ScoreManager::Destroy() {
	SDL_DestroyTexture(score_t);
	SDL_DestroyTexture(combo_t);
	SDL_DestroyTexture(score_text);
	SDL_DestroyTexture(combo_text);
	SDL_DestroyTexture(block_t);
	SDL_DestroyTexture(block_text);
}

void ScoreManager::Rendering(SDL_Renderer * renderer) {
	if (change_score == SCORE_CHANGE_SIZE) {
		SDL_DestroyTexture(score_t);
		SDL_DestroyTexture(combo_t);
		SDL_DestroyTexture(block_t);
		score_t = LoadText(to_string(score).c_str(), renderer, 70 * flexible, "main_font", 255, 255, 255);
		combo_t = LoadText(to_string(combo).c_str(), renderer, 50 * flexible, "main_font", 255, 255, 255);
		block_t = LoadText((to_string(crushed_blocks) + " / " + to_string(total_blocks)).c_str(), renderer, 50 * flexible, "main_font", 255, 255, 255);
	}
	SDL_Rect r;
	SDL_QueryTexture(score_text, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 2 - r.w / 2;
	r.y = 20 * flexible;
	SDL_RenderCopy(renderer, score_text, NULL, &r);

	SDL_QueryTexture(combo_text, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 4 - r.w / 2;
	r.y = 20 * flexible;
	SDL_RenderCopy(renderer, combo_text, NULL, &r);

	SDL_QueryTexture(block_text, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 4 * 3 - r.w / 2;
	r.y = 20 * flexible;
	SDL_RenderCopy(renderer, block_text, NULL, &r);


	SDL_QueryTexture(score_t, NULL, NULL, &r.w, &r.h);

	double c = change_score;
	r.w = (double)r.w * (c * c / 50.0 + 1.0);
	r.h = (double)r.h * (c * c / 50.0 + 1.0);
	r.x = win_w / 2 - r.w / 2;
	r.y = 100 * flexible - r.h / 2;
	SDL_RenderCopy(renderer, score_t, NULL, &r);

	SDL_QueryTexture(combo_t, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 4 - r.w / 2;
	r.y = 100 * flexible - r.h / 2;
	SDL_RenderCopy(renderer, combo_t, NULL, &r);

	SDL_QueryTexture(block_t, NULL, NULL, &r.w, &r.h);
	r.x = win_w / 4 * 3 - r.w / 2;
	r.y = 100 * flexible - r.h / 2;
	SDL_RenderCopy(renderer, block_t, NULL, &r);

	if (change_score != 0)
		change_score--;
}

void ScoreManager::AddResult(string s) {
	if (s == "PERFECT!!") {
		result.perfect++;
		combo++;
	}
	else if (s == "GREAT") {
		result.great++;
		combo = 0;
	}
	else if (s == "GOOD") {
		result.good++;
		combo = 0;
	}
	else if (s == "OK") {
		result.ok++;
		combo = 0;
	}
	else {
		result.miss++;
		combo = 0;
	}

	combo_score += combo;

	score = 0;
	score += result.perfect * 1000 + (int)(sqrt(combo_score) * 100) * 10;
	score += result.great * 1000;
	score += result.good * 500;
	score += result.ok * 100;

	change_score = SCORE_CHANGE_SIZE;
	crushed_blocks++;
	Background::ChangeColor(crushed_blocks);
}

Result ScoreManager::GetResult() {
	return result;
}

int ScoreManager::GetScore() {
	return score;
}

int ScoreManager::GetBlocks() {
	return total_blocks;
}
