#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "SetDataFile.h"
#include "Processor.h"
#include "MouseEffect.h"
#include "LoadTexture.h"
#include "Background.h"
#include "Info.h"
using namespace std;

SDL_Renderer *debug_ren;

string version = "v 1.1.0 - EOS Edition";
string music_to_play = "1";

int win_w;
int win_h;
double flexible;

int volume = 0;
int offset = 0;

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	srand(time(NULL));
	SDL_ShowCursor(SDL_DISABLE);

	SetDataFile();

	SDL_RWops *v_f;
	v_f = SDL_RWFromFile(Location(DAT, "", "volume.dat").c_str(), "r");
	volume = ReadNextInt(v_f, GetSize(Location(DAT, "", "volume.dat")));
	SDL_RWclose(v_f);
	Mix_VolumeMusic(volume * 128 / 100);

	v_f = SDL_RWFromFile(Location(DAT, "", "offset.dat").c_str(), "r");
	offset = ReadNextInt(v_f, GetSize(Location(DAT, "", "offset.dat")));
	SDL_RWclose(v_f);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	win_w = DM.w;
	win_h = DM.h;

	SDL_Window *window;
	if (DEBUGING) {
		win_w = 1200;
		win_h = 675;
		window = SDL_CreateWindow("Beat Crusher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, SDL_WINDOW_SHOWN);
	}
	else
		window = SDL_CreateWindow("Beat Crusher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, SDL_WINDOW_FULLSCREEN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	Block::crush_eff[0] = Mix_LoadWAV(Location(RES, "", "crush0.eff").c_str());
	Block::crush_eff[1] = Mix_LoadWAV(Location(RES, "", "crush1.eff").c_str());
	Block::crush_eff[2] = Mix_LoadWAV(Location(RES, "", "crush2.eff").c_str());

	if (win_w / 16 > win_h / 9)
		flexible = (double)win_h / 900.0;
	else
		flexible = (double)win_w / 1600.0;
	
	if(DEBUGING)
		debug_ren = renderer;

	MouseEffect mouse_effect(renderer);

	bool run = true;
	SDL_Event *event = new SDL_Event;

	Background::Init(renderer, "std");
	Background::ChangeAlpha(5);

	int rendering_timer = clock();
	int processing_uniform_timer = clock();

	Processor* processor;
	processor = new Processor;
	processor->SetMenuProcessor(renderer, event);
	//processor->SetGameProcessor(music_to_play, renderer, event, &mouse_effect);
	/*ScoreManager::Init(renderer, 10);
	processor->SetGameEndProcessor(renderer, event, music_to_play);*/

	int rendering_timing = 8;

	while (run) {
		while (SDL_PollEvent(event)) {
			switch (event->type) {
			case SDL_QUIT:
				run = false;
				break;
			case SDL_KEYUP:
				if (event->key.keysym.sym == SDLK_ESCAPE) {
					//run = false;
				}
				break;
			}
		}
		
		//Process (Erratic)
		processor->Processing_Erratic();

		//Process (Uniform)
		if (clock() - processing_uniform_timer >= 5) {
			mouse_effect.SetPoint();
			Background::Processing();
			processor->Processing_Uniform();
			int delta = clock() - processing_uniform_timer;
			if (delta > 6) {
				rendering_timing ++;
			}
			else if (rendering_timing > 6) {
				rendering_timing --;
			}
			processing_uniform_timer += 5;
		}

		//Rendering
		if (clock() - rendering_timer >= rendering_timing) { //60fps
			SDL_RenderClear(renderer);
			Background::Rendering(renderer);
			processor->Rendering();
			mouse_effect.Rendering(renderer);
			SDL_RenderPresent(renderer);
			
			rendering_timer += rendering_timing;
		}
		
		//Check Processor Finished
		if (processor->Finished()) {
			ProcessorType type;
			string msg;
			type = processor->GetType();
			msg = processor->GetDeadMessage();
			delete processor;

			processor = new Processor();
			switch (type) {
			case GAME_PROCESSOR:
				if (msg == "A") {
					processor->SetMusicListProcessor(renderer, event, stoi(music_to_play));
				}
				else {
					if (msg == "retry") {
						processor->SetGameProcessor(music_to_play, renderer, event, &mouse_effect, false);
					}
					else if (msg == "return") {
						processor->SetMenuProcessor(renderer, event);
					}
					else {
						processor->SetGameEndProcessor(renderer, event, music_to_play);
					}
				}
				break;
			case MENU_PROCESSOR:
				if (msg == "START") {
					processor->SetGameProcessor(music_to_play, renderer, event, &mouse_effect, false);
				}
				else if (msg == "EXIT") {
					run = false;
				}
				else if (msg == "MUSIC_LIST") {
					processor->SetMusicListProcessor(renderer, event, stoi(music_to_play));
				}
				break;
			case GAME_END_PROCESSOR:
				if (msg == "RETRY") {
					processor->SetGameProcessor(music_to_play, renderer, event, &mouse_effect, false);
				}
				else if (msg == "EXIT") {
					processor->SetMenuProcessor(renderer, event);
				}
				break;
			case MUSIC_LIST_PROCESSOR:
				if (msg.back() == 'A') {
					msg.pop_back();
					music_to_play = msg;
					processor->SetGameProcessor(music_to_play, renderer, event, &mouse_effect, true);
				}
				else {
					processor->SetMenuProcessor(renderer, event);
					music_to_play = msg;
				}
				break;
			}
		}
	}

	QuitLoadTextureLibrary();
	Mix_FreeChunk(Block::crush_eff[0]);
	Mix_FreeChunk(Block::crush_eff[1]);
	Mix_FreeChunk(Block::crush_eff[2]);
	Block::crush_eff[0] = nullptr;
	Block::crush_eff[1] = nullptr;
	Block::crush_eff[2] = nullptr;
	Background::Destroy();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}