#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include "ColorCalc.h"
#include "Button.h"
using namespace std;

struct Music {
	string name;
	string artist;
	int length;
	Color::RGB_Color theme_color;
	string score, rank;
	SDL_Texture *name_t, *artist_t, *length_t, *rank_t, *score_t;
};

class MusicListProcessor {
private:
	vector<Music> list;
	int selected_music, prev_music;

	Mix_Music *music;

	SDL_Renderer *renderer;
	SDL_Event *event;

	int lock;
	SDL_Texture *lock_t;

	bool finish;
	string dead_message;

	double move;

	Button *select_btn, *left_btn, *right_btn, *auto_play_btn;

	double open_effect_cnt, close_effect_cnt;

	void RenderingMusic(int n, int w, int a);

public:
	MusicListProcessor(SDL_Renderer *renderer, SDL_Event *event, int music);
	~MusicListProcessor();
	void Rendering();
	void Processing_Uniform();
	void Processing_Erratic();

	bool Finished();
	string GetDeadMessage();
};