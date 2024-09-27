#pragma once
#include <string>
#include <SDL.h>
#include "Button.h"
#include <SDL_mixer.h>
using namespace std;

class MenuProcessor {
private:
	bool finish;
	string dead_message;
	SDL_Renderer *renderer;
	SDL_Event *event;

	SDL_Texture *title_t, *version_t;

	double close_effect_cnt, open_effect_cnt;

	Button *start_btn, *close_btn, *music_list_btn, *setting_btn;

	Mix_Music *music;

	int background_effect_clock, background_effect;

	bool setting;

public:
	MenuProcessor(SDL_Renderer *renderer, SDL_Event *event);
	~MenuProcessor();
	void Rendering();
	void Processing_Uniform();
	void Processing_Erratic();

	bool Finished();
	string GetDeadMessage();
};

class Setting {
private:
	static int open_effect_cnt, close_effect_cnt;
	static string return_msg;
	static SDL_Texture *volume_t;
	static SDL_Texture *volume_text_t;
	static SDL_Texture* offset_t;
	static SDL_Texture* offset_text_t;
	static SDL_Texture *title_t;
	static Button *close_btn;
	static bool editing;
	static bool volume_editing;
	static bool offset_editing;

	static void ChangeVolume(int v, SDL_Renderer *renderer);
	static void ChangeOffset(int v, SDL_Renderer *renderer);

public:
	static void Init(SDL_Renderer *renderer);
	static void Destroy();
	static void Rendering(SDL_Renderer *renderer);
	static void Processing(SDL_Event *event, SDL_Renderer *renderer);
	static bool CheckSettingEditing();
};