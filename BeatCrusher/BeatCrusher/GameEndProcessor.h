#pragma once
#include "ScoreManager.h"
#include "Button.h"
#include <SDL.h>
#include <SDL_mixer.h>

enum Rank {BRONZE = 1, SILVER, GOLD, PLATINUM, DIAMOND, EMERALD, RUBY, MASTER};

class GameEndProcessor {
private:
	int score;
	Result result;
	SDL_Texture *rank_t, *score_t;
	SDL_Renderer *renderer;
	SDL_Event *event;
	Rank rank;

	Mix_Chunk *rank_eff;

	bool finish;

	Button *close_btn;
	Button *retry_btn;

	string dead_message;

	int end_step, step;

public:
	GameEndProcessor(SDL_Renderer *renderer, SDL_Event *event, string folder_name);
	~GameEndProcessor();
	void Rendering();
	void Processing_Erratic();
	void Processing_Uniform();

	bool Finished();
	string GetDeadMessage();
};

