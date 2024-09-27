#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include "Block.h"
using namespace std;

class BlockCreator {
private:
	Mix_Music *music;
	vector<int> data;
	vector<SDL_Point> pos;
	vector<Direction> dir;
	vector<SDL_Color> color;
	int next_time;
	bool playing;

public:
	int block_cnt;

	BlockCreator(string data, string pos, string dir, string music);
	~BlockCreator();
	void CreateBlock(vector<Block*> *blocks, bool auto_play);
	int NextTime();
};

