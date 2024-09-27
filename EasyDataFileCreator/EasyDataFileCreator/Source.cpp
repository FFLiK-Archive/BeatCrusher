#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <SDL_mixer.h>
#include <SDL.h>
#include <Windows.h>
using namespace std;

struct Time {
	float time;
	int loop;
};

void Save(vector<Time> time_table) {
	FILE *f;
	fopen_s(&f, "dat\\block.dat", "w");
	fprintf(f, "0 1");

	int cnt = 0;
	time_table[0].time -= 1000;
	for (int i = 0; i < time_table.size(); i++) {
		fprintf(f, "\n");
		fprintf(f, "%f ", time_table[i].time);
		fprintf(f, "%d", time_table[i].loop);
		cnt += time_table[i].loop;
	}
	fclose(f);

	fopen_s(&f, "dat\\pos.dat", "w");
	fprintf(f, "0 0");
	for (int i = 0; i < cnt; i++) {
		fprintf(f, "\n");
		fprintf(f, "%d %d", rand() % 5 + 1, rand() % 3 + 1);
	}
	fclose(f);

	fopen_s(&f, "dat\\dir.dat", "w");
	fprintf(f, "0 0");
	for (int i = 0; i < cnt; i++) {
		fprintf(f, "\n");
		fprintf(f, "2 2");
	}
	fclose(f);
}
#define W 0.25
float SetTime(float t, float beat) {
	double w = 1.0;

	if (t > beat) {
		while (!(t >= w * beat && t <= (w + W) * beat)) {
			w += W;
		}
		if (t - w * beat < (w + W) * beat - t) {
			return w * beat;
		}
		else {
			return (w + W) * beat;
		}
	}
	else {
		while (!(t <= w * beat && t >= (w - W) * beat)) {
			w -= W;
		}
		if (w * beat - t < t - (w - W) * beat) {
			return w * beat;
		}
		else {
			return (w - W) * beat;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 2048);

	SDL_Window *window = SDL_CreateWindow("Easy Creator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, 1);

	float bpm;
	int duration;
	FILE *f;
	fopen_s(&f, "dat\\bpm.txt", "r");
	fscanf_s(f, "%f", &bpm);
	fclose(f);
	fopen_s(&f, "dat\\duration.txt", "r");
	fscanf_s(f, "%d", &duration);
	fclose(f);

	float beat = 0;
	if (bpm != 0) {
		beat = 60000.0 / bpm;
	}
	else
		beat = 1;

	Mix_Music* music = Mix_LoadMUS("dat\\music.msc");
	Mix_Chunk *eff = Mix_LoadWAV("res\\crush0.eff");

	srand((unsigned)time(NULL));

	Sleep(1);

	
	SDL_Event event;

	vector<Time> time_table;

	bool run = true;

	int color = 0;

	int music_t = clock();

	int prev_time = 0;

	Mix_PlayMusic(music, 0);

	while (clock() - music_t <= duration * 1000 && run) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				float t = clock() - prev_time;
				//t = SetTime(t, beat);
				if (time_table.empty() || time_table.back().time != t) {
					cout << "?";
					Time p;
					p.time = t;
					p.loop = 1;
					time_table.push_back(p);
				}
				else {
					time_table.back().loop++;
				}
				cout << t << endl;
				prev_time = clock();
				color = 200;
			}
		}

		if ((clock() - music_t) % 16 == 0) {
			SDL_RenderClear(renderer);
			SDL_SetRenderDrawColor(renderer, color, color, color, 255);
			SDL_RenderPresent(renderer);
			if(color)
				color -= 2;
		}
	}

	if (run) {
		Save(time_table);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	Mix_FreeMusic(music);
	Mix_FreeChunk(eff);
	Mix_Quit();
	SDL_Quit();
	return 0;
}