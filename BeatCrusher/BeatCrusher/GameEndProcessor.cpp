#include "GameEndProcessor.h"
#include "LoadTexture.h"
#include "Background.h"
#include <iostream>
#include "RankDataCrypter.h"
#include "SetDataFile.h"
using namespace std;

extern int win_w, win_h;

extern double flexible;

#define MAX(x,y) (x > y ? x : y)

GameEndProcessor::GameEndProcessor(SDL_Renderer * renderer, SDL_Event * event, string folder_name) {
	this->event = event;
	this->renderer = renderer;
	this->result = ScoreManager::GetResult();
	this->score = ScoreManager::GetScore();
	this->step = 0;

	int blocks = ScoreManager::GetBlocks();

	double s = 0;
	s += (double)this->result.perfect / (double)blocks;
	s += (double)this->result.great * 0.8 / (double)blocks;
	s += (double)this->result.good * 0.5 / (double)blocks;
	s += (double)this->result.ok * 0.1 / (double)blocks;

	int max_score = 0;
	max_score = blocks * 1000 + (int)(sqrt(blocks * (blocks + 1) / 2) * 100) * 10;

	s = (double)this->score / max_score;

	this->end_step = 0;

	if (s == 1.0) {
		rank = MASTER;
		this->rank_t = LoadImage(Location(RES, "", "master.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 129, 61, 199 });
	}
	else if (s >= 0.9) {
		rank = RUBY;
		this->rank_t = LoadImage(Location(RES, "", "ruby.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 211, 71, 105 });
	}
	else if (s >= 0.8) {
		rank = EMERALD;
		this->rank_t = LoadImage(Location(RES, "", "emerald.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 112, 232, 201 });
	}
	else if (s >= 0.7) {
		rank = DIAMOND;
		this->rank_t = LoadImage(Location(RES, "", "diamond.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 149, 199, 204 });
	}
	else if (s >= 0.6) {
		rank = PLATINUM;
		this->rank_t = LoadImage(Location(RES, "", "platinum.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 228, 228, 228 });
	}
	else if (s >= 0.5) {
		rank = GOLD;
		this->rank_t = LoadImage(Location(RES, "", "gold.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 226, 186, 93 });
	}
	else if (s >= 0.3) {
		rank = SILVER;
		this->rank_t = LoadImage(Location(RES, "", "silver.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 166, 166, 166 });
	}
	else {
		rank = BRONZE;
		this->rank_t = LoadImage(Location(RES, "", "bronze.img").c_str(), this->renderer);
		Background::ChangeColorRGB({ 155, 111, 85 });
	}

	this->rank_eff = Mix_LoadWAV(Location(RES, "", "rank.eff").c_str());

	this->finish = false;
	this->dead_message = "";
	
	this->score_t = LoadText(to_string(this->score).c_str(), renderer, 100 * flexible, "main_font", 255, 255, 255);

	//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(this->score_t, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(this->rank_t, SDL_BLENDMODE_BLEND);

	this->close_btn = new Button(renderer, "EXIT >", { 1400, 800 }, 50, false, 255, 255, 255);
	this->retry_btn = new Button(renderer, "< RETRY", { 200, 800 }, 50, false, 255, 255, 255);

	SDL_RWops *f;
	int rr, ss;
	f = SDL_RWFromFile(Location(DAT, folder_name, "data.dat").c_str(), "r");
	int fsize = GetSize(Location(DAT, folder_name, "data.dat"));
	rr = ReadNextInt(f, fsize);
	ss = ReadNextInt(f, fsize);
	rr = IntDataDecrypter(rr);
	ss = IntDataDecrypter(ss);
	SDL_RWclose(f);

	f = SDL_RWFromFile(Location(DAT, folder_name, "data.dat").c_str(), "w");
	WriteInt(f, IntDataEncrypter(MAX(this->rank, rr)));
	SDL_RWwrite(f, " ", sizeof(char), 1);
	WriteInt(f, IntDataEncrypter(MAX(this->score, ss)));
	SDL_RWclose(f);
}

GameEndProcessor::~GameEndProcessor() {
	this->renderer = nullptr;
	this->event = nullptr;
	SDL_DestroyTexture(this->rank_t);
	SDL_DestroyTexture(this->score_t);
	this->rank_t = nullptr;
	this->score_t = nullptr;
	Mix_FreeChunk(this->rank_eff);
	delete this->close_btn;
	delete this->retry_btn;
}

void GameEndProcessor::Rendering() {

	int es = this->end_step * 0.4;
	int st = this->step * 0.4;

	if (es) {
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = win_w;
		r.h = win_h;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, (50 - es) * 2 + 1);
		SDL_RenderFillRect(renderer, &r);

		r.w = 600 * flexible + (es) * 5 * flexible;
		r.h = 600 * flexible + (es) * 5 * flexible;
		r.x = win_w / 2 - r.w / 2;
		r.y = win_h / 2 - r.h / 2 - 100 * flexible;
		SDL_SetTextureAlphaMod(this->rank_t, (int)((double)(50 - es) * 5.1));
		SDL_RenderCopy(renderer, this->rank_t, NULL, &r);

		SDL_SetTextureAlphaMod(this->score_t, (int)((double)(50 - es) * 5.1));
		SDL_QueryTexture(this->score_t, NULL, NULL, &r.w, &r.h);
		r.x = win_w / 2 - r.w / 2;
		r.y = 700 * flexible;
		SDL_RenderCopy(renderer, this->score_t, NULL, &r);
	}
	else {
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = win_w;
		r.h = win_h;
		if (st < 50) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, st * 2 + 1);
			SDL_RenderFillRect(renderer, &r);
		}
		else {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
			SDL_RenderFillRect(renderer, &r);
			if (st < 200 && st >= 100) {
				SDL_SetRenderDrawColor(renderer, Background::GetColor(60).r, Background::GetColor(60).g, Background::GetColor(60).b, (200 - st));
				SDL_RenderFillRect(renderer, &r);
			}
			if (st < 100) {
				if (st == 50) {
					Mix_PlayChannel(-1, this->rank_eff, 0);
				}
				r.w = 600 * flexible + (2401 - (st - 50) * (st - 50)) * flexible;
				r.h = 600 * flexible + (2401 - (st - 50) * (st - 50)) * flexible;
				r.x = win_w / 2 - r.w / 2;
				r.y = win_h / 2 - r.h / 2 - 100 * flexible;
				SDL_SetTextureAlphaMod(this->rank_t, (int)((double)(st - 50) * 5.1));
				SDL_RenderCopy(renderer, this->rank_t, NULL, &r);
			}
			else {
				if (st == 100) {
					SDL_SetTextureAlphaMod(this->rank_t, 255);
				}
				r.w = 600 * flexible;
				r.h = 600 * flexible;
				r.x = win_w / 2 - r.w / 2;
				r.y = win_h / 2 - r.h / 2 - 100 * flexible;
				SDL_RenderCopy(renderer, this->rank_t, NULL, &r);
				if (st < 200) {
					SDL_SetTextureAlphaMod(this->score_t, (int)((double)(st - 100) * 2.55));
					SDL_QueryTexture(this->score_t, NULL, NULL, &r.w, &r.h);
					r.x = win_w / 2 - r.w / 2;
					r.y = 700 * flexible;
				}
				else {
					if (st == 200)
						SDL_SetTextureAlphaMod(this->score_t, 255);
					SDL_QueryTexture(this->score_t, NULL, NULL, &r.w, &r.h);
					r.x = win_w / 2 - r.w / 2;
					r.y = 700 * flexible;
					this->close_btn->Rendering(renderer);
					this->retry_btn->Rendering(renderer);
				}
				SDL_RenderCopy(renderer, this->score_t, NULL, &r);
			}
		}
	}
}

void GameEndProcessor::Processing_Erratic() {
	if (this->step >= 200) {
		if (this->close_btn->CheckClick(*this->event)) {
			this->dead_message = "EXIT";
			this->end_step = 1;
		}
		else if (this->retry_btn->CheckClick(*this->event)) {
			this->dead_message = "RETRY";
			this->finish = true;
		}
	}
}

void GameEndProcessor::Processing_Uniform() {
	if (this->end_step) {
		end_step ++;
		if (end_step > 50 / 0.4)
			this->finish = true;
	}
	else {
		if (this->step <= 300 / 0.4)
			this->step ++;
	}
}

bool GameEndProcessor::Finished() {
	return this->finish;
}

string GameEndProcessor::GetDeadMessage() {
	return this->dead_message;
}
