#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Time.h"
#include "LoadTexture.h"
#include "Button.h"
#include "TimeEditer.h"
#include <ctime>
using namespace std;

void Rendering(SDL_Renderer *renderer, int alpha) {
	SDL_Rect r1 = { 10, 10, 960, 540 };
	SDL_Rect r2 = { 980, 10, 610, 260 };
	SDL_Rect r3 = { 980, 280, 610, 270 };
	SDL_Rect r4 = { 210, 560, 1480, 330 };
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
	SDL_RenderDrawRect(renderer, &r1);
	SDL_RenderDrawRect(renderer, &r2);
	SDL_RenderDrawRect(renderer, &r3);
	SDL_RenderDrawRect(renderer, &r4);
}

SDL_Renderer *debug_ren;

int win_w;
int win_h;

SDL_Texture* background;

void Load(vector<Time*> *time_table, SDL_Renderer *renderer) {
	time_table->clear();
	FILE *f;
	fopen_s(&f, "dat\\block.dat", "r");
	while (!feof(f)) {
		double a;
		int b;
		fscanf_s(f, "%lf", &a);
		fscanf_s(f, "%d", &b);
		if (a && b) {
			time_table->push_back(new Time(time_table->size(), renderer));
			time_table->back()->ChangeLoop(b, renderer);
			time_table->back()->ChangeTime(a, renderer);
		}
	}
	fclose(f);

	int a;
	fopen_s(&f, "dat\\pos.dat", "r");
	fscanf_s(f, "%d", &a);
	fscanf_s(f, "%d", &a);
	for (int i = 0; i < time_table->size() && !feof(f); i++) {
		(*time_table)[i]->block_pos->LoadPos(f);
	}
	fclose(f);

	fopen_s(&f, "dat\\dir.dat", "r");
	fscanf_s(f, "%d", &a);
	fscanf_s(f, "%d", &a);
	for (int i = 0; i < time_table->size() && !feof(f); i++) {
		(*time_table)[i]->block_pos->LoadDir(f);
	}
	fclose(f);
}

void Save(vector<Time*> *time_table) {
	FILE *f;
	fopen_s(&f, "dat\\block.dat", "w");
	fprintf(f, "0 1");
	for (int i = 0; i < time_table->size(); i++) {
		fprintf(f, "\n");
		if ((*time_table)[i]->time && (*time_table)[i]->loop) {
			fprintf(f, "%f ", (*time_table)[i]->time);
			fprintf(f, "%d", (*time_table)[i]->loop);
		}
	}
	fclose(f);

	fopen_s(&f, "dat\\pos.dat", "w");
	fprintf(f, "0 0");
	for (int i = 0; i < time_table->size(); i++) {
		(*time_table)[i]->block_pos->SavePos(f);
	}
	fclose(f);

	fopen_s(&f, "dat\\dir.dat", "w");
	fprintf(f, "0 0");
	for (int i = 0; i < time_table->size(); i++) {
		(*time_table)[i]->block_pos->SaveDir(f);
	}
	fclose(f);
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 2048);
	win_w = 1600;
	win_h = 900;
	SDL_Window *window = SDL_CreateWindow("Data File Creator - Beat Crusher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	bool run = true;
	SDL_Event event;

	vector<Time*> time_table;
	background = LoadImage("res\\background.img", renderer);

	int rendering_clock = clock();

	TimeEditer time_editer(renderer);

	Mix_Chunk *crush_eff = Mix_LoadWAV("res\\crush0.eff");

	BlockPosition::Init(renderer);

	Button start_btn, pause_btn, create_btn, move_left_btn, move_right_btn, delete_btn, save_btn, clear_btn;
	start_btn.MakeButton(renderer, "¢º", 220, 830, 100, 50, 30, 50, 50, 50, 200, 200, 200);
	pause_btn.MakeButton(renderer, "¡á", 330, 830, 100, 50, 30, 50, 50, 50, 200, 200, 200);
	create_btn.MakeButton(renderer, "create", 1170, 830, 200, 50, 30, 50, 50, 50, 200, 200, 200);
	delete_btn.MakeButton(renderer, "delete", 1380, 830, 200, 50, 30, 50, 50, 50, 200, 200, 200);
	move_left_btn.MakeButton(renderer, "¡ì", 1040, 830, 50, 50, 30, 50, 50, 50, 150, 150, 150);
	move_right_btn.MakeButton(renderer, "¡í", 1100, 830, 50, 50, 30, 50, 50, 50, 150, 150, 150);

	save_btn.MakeButton(renderer, "save", 800, 830, 200, 50, 30, 50, 50, 50, 200, 200, 200);
	clear_btn.MakeButton(renderer, "clear", 590, 830, 200, 50, 30, 50, 50, 50, 200, 200, 200);

	start_btn.Select(true);
	pause_btn.Select(false);
	create_btn.Select(true);
	delete_btn.Select(true);
	save_btn.Select(true);
	clear_btn.Select(true);

	int selected_time_table = 0;
	
	Load(&time_table, renderer);

	//FILE *bpm_f;
	//fopen_s(&bpm_f, "dat\\bpm.txt", "r");
	//float bpm = 0;
	//fscanf_s(bpm_f, "%f", &bpm);
	//fclose(bpm_f);
	float beat = 1;
	//if (bpm != 0) {
	//	beat = 60000.0 / bpm;
	//}
	//else
	//	beat = 1;

	if (time_table.empty()) {
		time_table.push_back(new Time(time_table.size(), renderer));
		float b = beat - 1000.0;
		if (b < 0) {
			b = 0;
		}
		time_table[selected_time_table]->ChangeTime(b, renderer);
	}

	time_editer.SetTime(time_table[selected_time_table]->time, renderer);
	time_editer.SetLoop(time_table[selected_time_table]->loop, renderer);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	int music_beat_square_alpha = 0;

	Mix_Music *music = nullptr;
	
	vector<double> beat_time_table;
	int beat_time_table_iter = 0;

	int beat_time = 0;
	bool music_run = false;

	while (run) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				run = false;
				break;
			case SDL_KEYUP:
				if (!music_run) {
					switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						if (selected_time_table) {
							selected_time_table--;
							for (int i = 0; i < time_table.size(); i++) {
								time_table[i]->pos++;
							}
						}
						time_editer.SetTime(time_table[selected_time_table]->time, renderer);
						time_editer.SetLoop(time_table[selected_time_table]->loop, renderer);
						break;
					case SDLK_RIGHT:
						if (selected_time_table < time_table.size() - 1) {
							selected_time_table++;
							for (int i = 0; i < time_table.size(); i++) {
								time_table[i]->pos--;
							}
						}
						time_editer.SetTime(time_table[selected_time_table]->time, renderer);
						time_editer.SetLoop(time_table[selected_time_table]->loop, renderer);
						break;
					}
				}
				break;
			}
			if (start_btn.Click(event)) {
				beat_time_table.clear();
				beat_time_table_iter = 0;
				double time = -1;
				double a = 0;
				int cnt = 0;
				int iter = 0;
				bool cancel = false;
				for (int i = 0; i < time_table.size(); i++) {
					time_table[i]->block_pos->ChangePage(renderer, 0);
					if (!time_table[i]->loop || !time_table[i]->time)
						cancel = true;
					for (int j = 0; j < time_table[i]->loop; j++) {
						if (i == selected_time_table && time == -1)
							time = a, iter = cnt;
						a += time_table[i]->time;
						beat_time_table.push_back(a + 1000);
						if (!j)
							beat_time_table.back() *= -1;
						cnt++;
					}
				}
				if (!cancel) {
					music_run = true;
					save_btn.Select(false);
					delete_btn.Select(false);
					create_btn.Select(false);
					clear_btn.Select(false);
					start_btn.Select(false);
					pause_btn.Select(true);
					music = Mix_LoadMUS("dat\\music.msc");
					beat_time_table_iter = iter;

					Mix_PlayMusic(music, 0);
					Mix_SetMusicPosition(time / 1000.0);
					Mix_VolumeMusic(64);

					selected_time_table = selected_time_table - 1;
					beat_time = clock() - time;
				}
			}
			else if (pause_btn.Click(event)) {
				music_run = false;
				Mix_HaltMusic();
				Mix_FreeMusic(music);
				start_btn.Select(true);
				pause_btn.Select(false);
				save_btn.Select(true);
				delete_btn.Select(true);
				create_btn.Select(true);
				clear_btn.Select(true);
				if (selected_time_table < 0)
					selected_time_table = 0;
				time_editer.SetTime(time_table[selected_time_table]->time / beat, renderer);
				time_editer.SetLoop(time_table[selected_time_table]->loop, renderer);
			}
			else if (create_btn.Click(event)) {
				time_table.insert(time_table.begin() + selected_time_table + 1,(new Time(time_table.size(), renderer)));
				selected_time_table++;
				int a = time_table[0]->pos;
				for (int i = 0; i < time_table.size(); i++) {
					time_table[i]->ChangeNum(i, renderer);
					time_table[i]->pos += a ;
				}
				time_editer.SetTime(time_table[selected_time_table]->time, renderer);
				time_editer.SetLoop(time_table[selected_time_table]->loop, renderer);
			}
			else if (delete_btn.Click(event)) {
				delete time_table[selected_time_table];
				time_table.erase(time_table.begin() + selected_time_table);
				selected_time_table--;
				if (selected_time_table < 0)
					selected_time_table = 0;
				if (time_table.empty()) {
					time_table.push_back(new Time(time_table.size(), renderer));
					float b = beat - 1000.0;
					if (b < 0) {
						b = 0;
					}
					time_table[selected_time_table]->ChangeTime(b, renderer);
				}
				int a = time_table[0]->pos;
				for (int i = 0; i < time_table.size(); i++) {
					time_table[i]->ChangeNum(i, renderer);
					time_table[i]->pos += a;
				}
				time_editer.SetTime(time_table[selected_time_table]->time, renderer);
				time_editer.SetLoop(time_table[selected_time_table]->loop, renderer);
			}
			else if (move_left_btn.Click(event)) {
				for (int i = 0; i < time_table.size(); i++) {
					time_table[i]->pos++;
				}
			}
			else if (move_right_btn.Click(event)) {
				for (int i = 0; i < time_table.size(); i++) {
					time_table[i]->pos--;
				}
			}
			else if (save_btn.Click(event)) {
				Save(&time_table);
			}
			else if (clear_btn.Click(event)) {
				for (int i = 0; i < time_table.size(); i++) {
					delete time_table.back();
					time_table.pop_back();
				}
				time_table.clear();
				time_table.push_back(new Time(time_table.size(), renderer));
				selected_time_table = 0;
				float b = beat - 1000.0;
				if (b < 0) {
					b = 0;
				}
				time_table[selected_time_table]->ChangeTime(b, renderer);
				time_editer.SetTime(time_table[selected_time_table]->time, renderer);
				time_editer.SetLoop(time_table[selected_time_table]->loop, renderer);
			}
			else if(!music_run) {
				if (time_editer.Event(event, renderer)) {
					if (!time_table.empty()) {
						time_table[selected_time_table]->ChangeTime(time_editer.GetTime() * beat - (selected_time_table ? 0 : 1000), renderer);
						time_table[selected_time_table]->ChangeLoop(time_editer.GetLoop(), renderer);
					}
				}

				time_table[selected_time_table]->block_pos->Event(event, renderer);
				for (int i = 0; i < time_table.size(); i++) {
					if (time_table[i]->Event(event)) {
						selected_time_table = i;
						time_table[i]->block_pos->ChangePage(renderer, 0);
						time_editer.SetTime((time_table[i]->time + (selected_time_table ? 0 : 1000)) / beat , renderer);
						time_editer.SetLoop(time_table[i]->loop, renderer);
					}
				}
			}
		}

		if (music_run) {
			if (beat_time_table_iter < beat_time_table.size()) {
				if (clock() - beat_time > abs(beat_time_table[beat_time_table_iter])) {
					music_beat_square_alpha = 255;
					Mix_PlayChannel(-1, crush_eff, 0);
					if (selected_time_table >= 0) {
						time_table[selected_time_table]->block_pos->ChangePage(renderer);
					}
					if (beat_time_table[beat_time_table_iter] < 0) {
						time_editer.SetTime(time_table[selected_time_table + 1]->time, renderer);
						time_editer.SetLoop(time_table[selected_time_table + 1]->loop, renderer);
						if (selected_time_table != -1) {
							for (int i = 0; i < time_table.size(); i++) {
								time_table[i]->pos--;
							}
						}
						selected_time_table++;
					}
					beat_time_table_iter++;
				}
			}
		}

		if (clock() - rendering_clock >= 16) {
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, background, NULL, NULL);
			start_btn.Rendering();
			pause_btn.Rendering();
			create_btn.Rendering();
			delete_btn.Rendering();
			clear_btn.Rendering();
			save_btn.Rendering();
			move_left_btn.Rendering();
			move_right_btn.Rendering();
			time_editer.Rendering(renderer);
			for (int i = 0; i < time_table.size(); i++) {
				if (time_table[i]->pos >= 0 && time_table[i]->pos < 8) {
					time_table[i]->Rendering(renderer, selected_time_table == i);
				}
			}
			if (selected_time_table >= 0) {
				time_table[selected_time_table]->block_pos->Rendering(renderer, music_run, music_beat_square_alpha);
			}
			Rendering(renderer, music_beat_square_alpha);

			SDL_RenderPresent(renderer);
			rendering_clock = clock();
		}
		
		if (music_beat_square_alpha)
			music_beat_square_alpha -= 1;
		SDL_Delay(1);
	}

	Mix_FreeChunk(crush_eff);
	BlockPosition::Destroy();
	SDL_DestroyTexture(background);
	clear_btn.Destroy();
	save_btn.Destroy();
	create_btn.Destroy();
	delete_btn.Destroy();
	start_btn.Destroy();
	pause_btn.Destroy();
	move_left_btn.Destroy();
	move_right_btn.Destroy();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}