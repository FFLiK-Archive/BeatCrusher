#pragma once
#include "GameProcessor.h"
#include "GameEndProcessor.h"
#include "MenuProcessor.h"
#include "MusicListProcessor.h"
#include <SDL.h>

enum ProcessorType {UNDEFINED_PROCESSOR, GAME_PROCESSOR, MENU_PROCESSOR, GAME_END_PROCESSOR, MUSIC_LIST_PROCESSOR};
class Processor {
private:
	ProcessorType type;
	GameProcessor *game;
	GameEndProcessor *game_end;
	MenuProcessor *menu;
	MusicListProcessor *music_list;

public:
	Processor();
	~Processor();

	void SetGameProcessor(string folder_name, SDL_Renderer *renderer, SDL_Event *event, MouseEffect *mouse_effect, bool auto_play);
	void SetMenuProcessor(SDL_Renderer * renderer, SDL_Event * event);
	void SetGameEndProcessor(SDL_Renderer *renderer, SDL_Event *event, string folder_name);
	void SetMusicListProcessor(SDL_Renderer *renderer, SDL_Event *event, int music);

	void Rendering();
	void Processing_Uniform();
	void Processing_Erratic();

	bool Finished();

	void Destroy();

	ProcessorType GetType();
	string GetDeadMessage();
};

