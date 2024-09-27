#include "Processor.h"

Processor::Processor() {
	this->type = UNDEFINED_PROCESSOR;
	this->game = nullptr;
	this->game_end = nullptr;
	this->menu = nullptr;
	this->music_list = nullptr;
}

Processor::~Processor() {
	this->Destroy();
}

void Processor::SetGameProcessor(string folder_name, SDL_Renderer * renderer, SDL_Event *event, MouseEffect * mouse_effect, bool auto_play) {
	this->type = GAME_PROCESSOR;
	this->game = new GameProcessor(folder_name, renderer, event, mouse_effect, auto_play);
}

void Processor::SetMenuProcessor(SDL_Renderer * renderer, SDL_Event * event) {
	this->type = MENU_PROCESSOR;
	this->menu = new MenuProcessor(renderer, event);
}

void Processor::SetGameEndProcessor(SDL_Renderer * renderer, SDL_Event * event, string folder_name) {
	this->type = GAME_END_PROCESSOR;
	this->game_end = new GameEndProcessor(renderer, event, folder_name);
}

void Processor::SetMusicListProcessor(SDL_Renderer * renderer, SDL_Event * event, int music) {
	this->type = MUSIC_LIST_PROCESSOR;
	this->music_list = new MusicListProcessor(renderer, event, music);
}

void Processor::Rendering() {
	switch (this->type) {
	case GAME_PROCESSOR:
		this->game->Rendering();
		break;
	case MENU_PROCESSOR:
		this->menu->Rendering();
		break;
	case GAME_END_PROCESSOR:
		this->game_end->Rendering();
		break;
	case MUSIC_LIST_PROCESSOR:
		this->music_list->Rendering();
		break;
	}
}

void Processor::Processing_Uniform() {
	switch (this->type) {
	case GAME_PROCESSOR:
		this->game->Processing_Uniform();
		break;
	case MENU_PROCESSOR:
		this->menu->Processing_Uniform();
		break;
	case GAME_END_PROCESSOR:
		this->game_end->Processing_Uniform();
		break;
	case MUSIC_LIST_PROCESSOR:
		this->music_list->Processing_Uniform();
		break;
	}
}

void Processor::Processing_Erratic() {
	switch (this->type) {
	case GAME_PROCESSOR:
		this->game->Processing_Erratic();
		break;
	case MENU_PROCESSOR:
		this->menu->Processing_Erratic();
		break;
	case GAME_END_PROCESSOR:
		this->game_end->Processing_Erratic();
		break;
	case MUSIC_LIST_PROCESSOR:
		this->music_list->Processing_Erratic();
	}
}

bool Processor::Finished() {
	switch (this->type) {
	case GAME_PROCESSOR:
		return this->game->Finished();
	case MENU_PROCESSOR:
		return this->menu->Finished();
	case GAME_END_PROCESSOR:
		return this->game_end->Finished();
	case MUSIC_LIST_PROCESSOR:
		return this->music_list->Finished();
	}
	return false;
}

void Processor::Destroy() {
	if (this->game)
		delete this->game;
	this->game = nullptr;

	if (this->game_end)
		delete this->game_end;
	this->game_end = nullptr;

	if (this->menu)
		delete this->menu;
	this->menu = nullptr;

	if (this->music_list)
		delete this->music_list;
	this->music_list = nullptr;

	this->type = UNDEFINED_PROCESSOR;
}

ProcessorType Processor::GetType() {
	return this->type;
}

string Processor::GetDeadMessage() {
	switch (this->type) {
	case GAME_PROCESSOR:
		return this->game->GetDeadMessage();
	case MENU_PROCESSOR:
		return this->menu->GetDeadMessage();
	case GAME_END_PROCESSOR:
		return this->game_end->GetDeadMessage();
	case MUSIC_LIST_PROCESSOR:
		return this->music_list->GetDeadMessage();
	}
	return "";
}
