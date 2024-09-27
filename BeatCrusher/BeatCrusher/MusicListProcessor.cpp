#include "MusicListProcessor.h"
#include "LoadTexture.h"
#include "Background.h"
#include "RankDataCrypter.h"
#include "SetDataFile.h"
using namespace Color;

extern double flexible;
extern int win_w, win_h;

#define EFFECT_MAX 30

#define MAX(x, y) (x > y ? x : y)

MusicListProcessor::MusicListProcessor(SDL_Renderer * renderer, SDL_Event *event, int music) {
	this->renderer = renderer;
	this->event = event;
	this->selected_music = music - 1;
	this->dead_message = "";
	this->finish = false;

	int music_cnt;
	SDL_RWops *f;
	f = SDL_RWFromFile(Location(DAT, "std", "music.dat").c_str(), "r");
	int fsize = GetSize(Location(DAT, "std", "music.dat"));
	music_cnt = ReadNextInt(f, fsize);
	this->lock = ReadNextInt(f, fsize);
	SDL_RWclose(f);

	Music m;
	for (int i = 1; i <= music_cnt; i++) {
		f = SDL_RWFromFile(Location(DAT, to_string(i), "info.dat").c_str(), "r");
		fsize = GetSize(Location(DAT, to_string(i), "info.dat"));
		m.name = ReadNextLine(f, fsize);
		for (int j = 0; j < m.name.size(); j++) {
			if (m.name[j] == '_') m.name[j] = ' ';
		}
		m.artist = ReadNextLine(f, fsize);
		for (int j = 0; j < m.artist.size(); j++) {
			if (m.artist[j] == '_') m.artist[j] = ' ';
		}
		m.length = ReadNextInt(f, fsize);
		m.theme_color.r = ReadNextInt(f, fsize);
		m.theme_color.g = ReadNextInt(f, fsize);
		m.theme_color.b = ReadNextInt(f, fsize);
		SDL_RWclose(f);

		f = SDL_RWFromFile(Location(DAT, to_string(i), "data.dat").c_str(), "r");
		fsize = GetSize(Location(DAT, to_string(i), "data.dat"));
		int n = ReadNextInt(f, fsize);

		n = IntDataDecrypter(n);
		
		switch (n) {
		case 0:
			m.rank = "UNKNOWN";
			break;
		case 1:
			m.rank = "BRONZE";
			break;
		case 2:
			m.rank = "SILVER";
			break;
		case 3:
			m.rank = "GOLD";
			break;
		case 4:
			m.rank = "PLATINUM";
			break;
		case 5:
			m.rank = "DIAMOND";
			break;
		case 6:
			m.rank = "EMERALD";
			break;
		case 7:
			m.rank = "RUBY";
			break;
		case 8:
			m.rank = "MASTER";
			break;
		}

		n = ReadNextInt(f, fsize);
		n = IntDataDecrypter(n);

		m.score = to_string(n);
		SDL_RWclose(f);

		RGB_Color theme;
		HSV_Color hsv = RGBtoHSV(m.theme_color);
		hsv.v += 50;
		if (hsv.v > 100)
			hsv.v = 100;
		if (hsv.v < 0)
			hsv.v = 0;
		theme = HSVtoRGB(hsv);

		m.name_t = LoadText(m.name.c_str(), renderer, 50 * flexible, "main_font", theme.r, theme.g, theme.b);
		m.artist_t = LoadText(("Artist | " + m.artist).c_str(), renderer, 30 * flexible, "main_font", 255, 255, 255);
		m.length_t = LoadText(("Length | " + to_string(m.length / 60) + " : " + to_string(m.length % 60)).c_str(), renderer, 30 * flexible, "main_font", 255, 255, 255);
		m.rank_t = LoadText(("Rank | " + m.rank).c_str(), renderer, 30 * flexible, "main_font", theme.r, theme.g, theme.b);
		m.score_t = LoadText(("Score | " + m.score).c_str(), renderer, 30 * flexible, "main_font", theme.r, theme.g, theme.b);
		this->list.push_back(m);
	}

	this->left_btn = new Button(renderer, "<", { 250, 400 }, 200, false, 255, 255, 255, 100);
	this->right_btn = new Button(renderer, ">", { 1350, 400 }, 200, false, 255, 255, 255, 100);
	this->select_btn = new Button(renderer, "Select", { 800, 800 }, 70, false, 255, 255, 255);
	this->auto_play_btn = new Button(renderer, "Auto Play", { 1400, 800 }, 40, false, 255, 255, 255, 200);

	this->lock_t = LoadText("LOCKED", renderer, 100 * flexible, "main_font", 255, 255, 255);

	if(!this->list.empty())
		Background::ChangeColorRGB(this->list[this->selected_music].theme_color);

	this->music = Mix_LoadMUS(Location(DAT, to_string(this->selected_music + 1), "music.msc").c_str());
	Mix_PlayMusic(this->music, 0);

	this->open_effect_cnt = EFFECT_MAX, this->close_effect_cnt = 0;

	this->move = 0;
}

MusicListProcessor::~MusicListProcessor() {
	for (int i = 0; i < this->list.size(); i++) {
		SDL_DestroyTexture(this->list[i].artist_t);
		SDL_DestroyTexture(this->list[i].length_t);
		SDL_DestroyTexture(this->list[i].name_t);
		SDL_DestroyTexture(this->list[i].rank_t);
		SDL_DestroyTexture(this->list[i].score_t);
	}
	this->list.clear();
	Mix_HaltMusic();
	Mix_FreeMusic(this->music);

	delete this->auto_play_btn;
	delete this->left_btn;
	delete this->right_btn;
	delete this->select_btn;
}

void MusicListProcessor::RenderingMusic(int n, int w, int a) {
	int m = MAX(this->close_effect_cnt * this->close_effect_cnt, this->open_effect_cnt * this->open_effect_cnt) * flexible;
	
	SDL_SetTextureAlphaMod(this->list[n].artist_t, a);
	SDL_SetTextureAlphaMod(this->list[n].length_t, a);
	SDL_SetTextureAlphaMod(this->list[n].name_t, a);
	SDL_SetTextureAlphaMod(this->list[n].rank_t, a);
	SDL_SetTextureAlphaMod(this->list[n].score_t, a);

	SDL_Rect space;
	space.w = 800 * flexible;
	space.h = 600 * flexible;
	space.x = win_w / 2 - space.w / 2 + w * flexible;
	space.y = win_h / 2 - space.h / 2 - 50 * flexible - m;
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, a / 2);
	SDL_RenderFillRect(this->renderer, &space);

	SDL_QueryTexture(this->list[n].name_t, NULL, NULL, &space.w, &space.h);
	space.x = win_w / 2 - space.w / 2 + w * flexible;
	space.y = 150 * flexible - m;
	SDL_RenderCopy(this->renderer, this->list[n].name_t, NULL, &space);

	int x_pos = 500;
	int y_pos = 280 - m;
	int y_increase = 70;

	SDL_QueryTexture(this->list[n].artist_t, NULL, NULL, &space.w, &space.h);
	space.x = x_pos * flexible + w * flexible;
	space.y = (y_pos + y_increase * 0) * flexible;
	SDL_RenderCopy(this->renderer, this->list[n].artist_t, NULL, &space);

	SDL_QueryTexture(this->list[n].length_t, NULL, NULL, &space.w, &space.h);
	space.x = x_pos * flexible + w * flexible;
	space.y = (y_pos + y_increase * 1) * flexible;
	SDL_RenderCopy(this->renderer, this->list[n].length_t, NULL, &space);

	SDL_QueryTexture(this->list[n].rank_t, NULL, NULL, &space.w, &space.h);
	space.x = x_pos * flexible + w * flexible;
	space.y = (y_pos + y_increase * 3) * flexible;
	SDL_RenderCopy(this->renderer, this->list[n].rank_t, NULL, &space);

	SDL_QueryTexture(this->list[n].score_t, NULL, NULL, &space.w, &space.h);
	space.x = x_pos * flexible + w * flexible;
	space.y = (y_pos + y_increase * 4) * flexible;
	SDL_RenderCopy(this->renderer, this->list[n].score_t, NULL, &space);

	if (n >= this->lock - 1) {
		SDL_QueryTexture(this->lock_t, NULL, NULL, &space.w, &space.h);
		space.x = win_w / 2 - space.w / 2 + w * flexible;
		space.y = win_h / 2 - space.h / 2 - 50 * flexible - m;
		SDL_SetTextureAlphaMod(this->lock_t, 100);
		SDL_RenderCopy(this->renderer, this->lock_t, NULL, &space);
	}
}

#define MOVE 50

void MusicListProcessor::Rendering() {
	int m = MAX(this->close_effect_cnt * this->close_effect_cnt, this->open_effect_cnt * this->open_effect_cnt) * flexible;

	this->left_btn->MoveButton({ 250, 400 - m });
	this->right_btn->MoveButton({ 1350, 400 - m });
	this->select_btn->MoveButton({ 800, 800 + m });
	this->auto_play_btn->MoveButton({ 1400, 800 + m });

	if (!this->list.empty()) {
		int change_pos = this->move * this->move * this->move * 1000 / (MOVE * MOVE * MOVE);
		int change_alpha = this->move * this->move * this->move * 200 / (MOVE * MOVE * MOVE);
		if (this->selected_music > 0) {
			this->RenderingMusic(this->selected_music - 1, -1000 + change_pos, 50 + abs(change_alpha));
			if(!this->move)
				this->left_btn->Rendering(this->renderer);
		}
		if (this->selected_music < this->list.size() - 1) {
			this->RenderingMusic(this->selected_music + 1, 1000 + change_pos, 50 + abs(change_alpha));
			if(!this->move)
				this->right_btn->Rendering(this->renderer);
		}
		this->RenderingMusic(this->selected_music, 0 + change_pos, 250 - abs(change_alpha));
	}

	if (this->selected_music < this->lock - 1) {
		this->select_btn->Rendering(renderer);
		this->auto_play_btn->Rendering(renderer);
	}
}

void MusicListProcessor::Processing_Uniform() {
	if (this->close_effect_cnt)
		this->close_effect_cnt += 0.4;

	if (this->close_effect_cnt > EFFECT_MAX)
		this->finish = true;

	if (this->open_effect_cnt > 0)
		this->open_effect_cnt -= 0.4;
	else
		this->open_effect_cnt = 0;

	if (this->move) {
		if (this->move > 0)
			this->move -= 0.4;
		else
			this->move += 0.4;

		if (abs(this->move) < 0.01) {
			this->move = 0;
			Mix_HaltMusic();
			Mix_FreeMusic(this->music);
			this->music = Mix_LoadMUS(Location(DAT, to_string(this->selected_music + 1), "music.msc").c_str());
			Mix_PlayMusic(this->music, 0);
		}
	}
}

void MusicListProcessor::Processing_Erratic() {
	if (!this->close_effect_cnt && !this->open_effect_cnt) {
		if (!this->list.empty() && !this->move) {
			if (this->left_btn->CheckClick(*this->event)) {
				if (this->selected_music > 0) {
					this->prev_music = this->selected_music;
					this->selected_music--;
					Background::ChangeColorRGB(this->list[this->selected_music].theme_color);
					this->move = -MOVE;
					Mix_FadeOutMusic((MOVE - 10) * 15);
				}
			}
			if (this->right_btn->CheckClick(*this->event)) {
				if (this->selected_music < this->list.size() - 1) {
					this->prev_music = this->selected_music;
					this->selected_music++;
					Background::ChangeColorRGB(this->list[this->selected_music].theme_color);
					this->move = MOVE;
					Mix_FadeOutMusic((MOVE - 10) * 15);
				}
			}
		}
		if (this->selected_music < this->lock - 1) {
			if (this->select_btn->CheckClick(*this->event)) {
				this->close_effect_cnt = 1;
				this->dead_message = to_string(this->selected_music + 1);
				Mix_FadeOutMusic(500);
			}
			else if (this->auto_play_btn->CheckClick(*this->event)) {
				this->close_effect_cnt = 1;
				this->dead_message = to_string(this->selected_music + 1) + "A";
				Mix_FadeOutMusic(500);
			}
		}
	}
}

bool MusicListProcessor::Finished() {
	return this->finish;
}

string MusicListProcessor::GetDeadMessage() {
	return this->dead_message;
}
