#include "GameProcessor.h"
#include "SetDataFile.h"

extern double flexible;
extern int win_w, win_h;

extern int offset;

GameProcessor::GameProcessor(string folder_name, SDL_Renderer *renderer, SDL_Event *event, MouseEffect *mouse_effect, bool auto_play) {
	this->block_creator = new BlockCreator(Location(DAT, folder_name, "block.dat"), 
		Location(DAT, folder_name, "pos.dat"), 
		Location(DAT, folder_name, "dir.dat"), 
		Location(DAT, folder_name, "music.msc"));

	this->pause_btn = new Button(renderer, "PAUSE", { 1480, 820 }, 40, true, 255, 255, 255, 200);

	this->delay = 0;

	this->pause = false;

	SDL_RWops *f;
	f = SDL_RWFromFile(Location(DAT, folder_name, "info.dat").c_str(), "r");
	int fsize = GetSize(Location(DAT, folder_name, "info.dat"));
	int len;
	ReadNextLine(f, fsize);
	ReadNextLine(f, fsize);
	len = ReadNextInt(f, fsize);
	SDL_RWclose(f);

	this->music_len = len * 1000;

	ScoreManager::Init(renderer, this->block_creator->block_cnt - 1);
	this->mouse_effect = mouse_effect;
	this->mouse_effect->GetMousePosition(&Block::mouse_pos, &Block::prev_mouse_pos, 0);

	this->block_create_time = -1;

	this->block_crush_delay = 0;

	this->music_playing = false;

	this->renderer = renderer;
	this->event = event;

	this->finish = false;
	this->dead_message = "";

	this->auto_play = auto_play;
	if (this->auto_play)
		this->dead_message = "A";

	Background::NewBackground(folder_name);
	Background::ChangeAlpha(5);

	TextEffect::Init();

	Pause::Init(this->renderer, this->auto_play);
}

GameProcessor::~GameProcessor() {
	ScoreManager::Destroy();
	Pause::Destroy();
	for (int i = 0; i < this->blocks.size(); i++) {
		delete this->blocks[i];
		this->blocks[i] = nullptr;
	}
	this->blocks.clear();
	delete this->block_creator;
}

void GameProcessor::Rendering() {
	for (int i = this->blocks.size() - 1; i >= 0; i--) {
		this->blocks[i]->Rendering(renderer);
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	ScoreManager::Rendering(this->renderer);
	TextEffect::Rendering(this->renderer);

	this->pause_btn->Rendering(this->renderer);

	if (this->pause)
		Pause::Rendering(this->renderer);

	/*for (int j = 0; j < 10 && !crashed; j++) {
		mouse_effect.GetMousePosition(&Block::mouse_pos, &Block::prev_mouse_pos, j);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		DrawLine(Block::mouse_pos, Block::prev_mouse_pos, renderer, 25, 255, 0,0,255);
	}*/
}

void GameProcessor::Processing_Uniform() {
	if (!this->pause) {
		for (int i = 0; i < blocks.size(); i++) {
			blocks[i]->Move();
		}
	}
	TextEffect::Processing();
}

void GameProcessor::Processing_Erratic() {
	bool crashed = false;
	for (int j = 0; j < 10 && !crashed; j++) {
		this->mouse_effect->GetMousePosition(&Block::mouse_pos, &Block::prev_mouse_pos, j);
		for (int i = 0; i < this->blocks.size(); i++) {
			if (this->blocks[i]->Processing(clock() - block_crush_delay > 100 && !this->pause, !j)) {
				this->block_crush_delay = clock();
				crashed = true;
				break;
			}
			this->blocks[i]->Able();
			if (this->blocks[i]->destroyed) {
				delete this->blocks[i];
				this->blocks.erase(blocks.begin() + i);
			}
		}
	}

	if (!this->pause) {
		if ((clock() - this->block_create_time - delay - offset > this->block_creator->NextTime() || !this->music_playing) && this->block_creator->NextTime() != -1) {
			this->block_creator->CreateBlock(&this->blocks, this->auto_play);
			if (!this->music_playing)
				this->block_create_time = clock(), this->music_playing = true;
		}

		if (clock() - this->block_create_time > this->music_len) {
			if (this->blocks.empty()) {
				this->music_playing = false;
				this->finish = true;
			}
		}

		if (pause_btn->CheckClick(*this->event)) {
			this->pause = true;
			Mix_PauseMusic();
			Pause::PauseGame(this);
		}
	}
	else {
		Pause::Processing(this->event);
	}
}

bool GameProcessor::Finished() {
	return this->finish;
}

string GameProcessor::GetDeadMessage() {
	return this->dead_message;
}

void GameProcessor::Resume(string mode) {
	if (mode == "resume") {
		this->pause = false;
		int d = clock() - Pause::pause_start_time;
		this->delay += d;
		for (int i = 0; i < this->blocks.size(); i++) {
			this->blocks[i]->Delay(d);
		}
		Mix_ResumeMusic();
	}
	else if (mode == "retry") {
		this->finish = true;
		this->dead_message = "retry";
	}
	else if (mode == "return") {
		this->finish = true;
		this->dead_message = "return";
	}
}

int Pause::pause_start_time;
int Pause::open_effect_cnt, Pause::close_effect_cnt;
Button *Pause::resume_btn, *Pause::retry_btn, *Pause::return_btn;
GameProcessor *Pause::game;
string Pause::return_msg;
int Pause::timer, Pause::timer_clock;
SDL_Texture *Pause::timer_t;
bool Pause::auto_play;

#define EFFECT_MAX 30

void Pause::Init(SDL_Renderer *renderer, bool AUTO) {
	pause_start_time = 0;
	open_effect_cnt = 0;
	close_effect_cnt = 0;
	auto_play = AUTO;
	resume_btn = new Button(renderer, "Continue", { 800, 300 }, 70, false, 255, 255, 255, 255);
	retry_btn = new Button(renderer, "Retry", { 800, 450 }, 70, false, 255, 255, 255, 255);
	return_btn = new Button(renderer, "Back to Menu", { 800, 600 }, 70, false, 255, 255, 255, 255);
	game = nullptr;
}

void Pause::Destroy() {
	game = nullptr;
	delete resume_btn;
	delete retry_btn;
	delete return_btn;
}

void Pause::PauseGame(GameProcessor *game_processor) {
	pause_start_time = clock();
	open_effect_cnt = EFFECT_MAX;
	close_effect_cnt = 0;
	game = game_processor;
	timer_t = nullptr;
}

void Pause::Rendering(SDL_Renderer * renderer) {
	int m = 0;
	int a = 0;
	if (open_effect_cnt) {
		m = open_effect_cnt;
		open_effect_cnt--;
	}
	else if (close_effect_cnt && close_effect_cnt <= EFFECT_MAX) {
		m = close_effect_cnt;
		close_effect_cnt++;
	}
	a = m;
	m = m * m * flexible;

	resume_btn->MoveButton({ 800, 300 - m });
	retry_btn->MoveButton({ 800, 450 - m });
	return_btn->MoveButton({ 800, 600 - m });

	if (close_effect_cnt <= EFFECT_MAX) {

		SDL_Rect r;
		r.w = win_w;
		r.h = win_h;
		r.x = 0;
		r.y = 0;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100 - a * 100 / EFFECT_MAX);
		SDL_RenderFillRect(renderer, &r);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
		r.w = 800 * flexible;
		r.h = 500 * flexible;
		r.x = win_w / 2 - r.w / 2;
		r.y = win_h / 2 - r.h / 2 - m;
		SDL_RenderFillRect(renderer, &r);

		resume_btn->Rendering(renderer);
		if(!auto_play) retry_btn->Rendering(renderer);
		return_btn->Rendering(renderer);

		if (close_effect_cnt == EFFECT_MAX) {
			timer_clock = clock();
			timer = 3;
			timer_t = LoadText(to_string(timer).c_str(), renderer, 200 * flexible, "main_font", 255, 255, 255);
		}
	}
	else {
		if (return_msg == "resume") {
			if (timer_clock + 1000 <= clock()) {
				timer_clock = clock();
				timer--;
				SDL_DestroyTexture(timer_t);
				if (timer == 0)
					game->Resume(return_msg);
				timer_t = LoadText(to_string(timer).c_str(), renderer, 200 * flexible, "main_font", 255, 255, 255);
			}
		}
		else {
			game->Resume(return_msg);
		}
	}

	if (timer_t) {
		SDL_Rect r;
		SDL_SetTextureAlphaMod(timer_t, 100);
		SDL_QueryTexture(timer_t, NULL, NULL, &r.w, &r.h);
		r.x = win_w / 2 - r.w / 2;
		r.y = win_h / 2 - r.h / 2 - m;
		SDL_RenderCopy(renderer, timer_t, NULL, &r);
	}
}

void Pause::Processing(SDL_Event * event) {
	if (resume_btn->CheckClick(*event)) {
		return_msg = "resume";
		close_effect_cnt = 1;
	}
	else if (retry_btn->CheckClick(*event) && !auto_play) {
		return_msg = "retry";
		close_effect_cnt = 1;
	}
	else if (return_btn->CheckClick(*event)) {
		return_msg = "return";
		close_effect_cnt = 1;
	}
}
