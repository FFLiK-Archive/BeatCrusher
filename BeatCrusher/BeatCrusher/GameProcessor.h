#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "MouseEffect.h"
#include "LoadTexture.h"
#include "Block.h"
#include "Background.h"
#include "ScoreManager.h"
#include "BlockCreator.h"
#include "Button.h"

class GameProcessor {
private:
	SDL_Renderer *renderer;
	vector<Block*> blocks;
	MouseEffect *mouse_effect;
	BlockCreator *block_creator;
	SDL_Event *event;
	
	int block_create_time, block_crush_delay, delay;
	bool music_playing;
	
	bool finish;
	string dead_message;

	bool auto_play;

	int music_len;

	Button *pause_btn;

	bool pause;

public:
	GameProcessor(string folder_name, SDL_Renderer *renderer, SDL_Event *event, MouseEffect *mouse_effect, bool auto_play);
	~GameProcessor();
	void Rendering();
	void Processing_Uniform();
	void Processing_Erratic();

	bool Finished();
	string GetDeadMessage();

	void Resume(string mode);
};


class Pause {
private:
	static int open_effect_cnt, close_effect_cnt;
	static Button *resume_btn, *retry_btn, *return_btn;
	static GameProcessor *game;
	static string return_msg;
	static int timer, timer_clock;
	static SDL_Texture *timer_t;
	static bool auto_play;

public:
	static int pause_start_time;
	static void Init(SDL_Renderer *renderer, bool AUTO);
	static void Destroy();
	static void PauseGame(GameProcessor *game_processor);
	static void Rendering(SDL_Renderer *renderer);
	static void Processing(SDL_Event *event);
};