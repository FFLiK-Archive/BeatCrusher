#include "BlockCreator.h"
#include "ScoreManager.h"
#include "SetDataFile.h"
#include <iostream>

extern int win_w, win_h;
extern double flexible;
extern int volume;

BlockCreator::BlockCreator(string data, string pos, string dir, string music) {

	Mix_VolumeChunk(Block::crush_eff[0], volume * 128 / 100);
	Mix_VolumeChunk(Block::crush_eff[1], volume * 128 / 100);
	Mix_VolumeChunk(Block::crush_eff[2], volume * 128 / 100);

	this->music = Mix_LoadMUS(music.c_str());
	SDL_RWops *f;
	f = SDL_RWFromFile(data.c_str(), "r");
	float a = 0;
	int fsize = GetSize(data);
	while (SDL_RWseek(f, 0, RW_SEEK_CUR) != fsize) {
		float d;
		int t;
		d = ReadNextFloat(f, fsize);
		t = ReadNextInt(f, fsize);
		for (int i = 0; i < t; i++) {
			a += d;
			this->data.push_back(a);
		}
	}
	SDL_RWclose(f);
	
	this->block_cnt = this->data.size();

	f = SDL_RWFromFile(pos.c_str(), "r");
	fsize = GetSize(pos);
	while (SDL_RWseek(f, 0, RW_SEEK_CUR) != fsize) {
		int a, b;
		a = ReadNextInt(f, fsize);
		b = ReadNextInt(f, fsize);
		this->pos.push_back({ a,b });
	}
	SDL_RWclose(f);

	if (this->block_cnt > this->pos.size())
		this->block_cnt = this->pos.size();

	f = SDL_RWFromFile(dir.c_str(), "r");
	fsize = GetSize(dir);
	while (SDL_RWseek(f, 0, RW_SEEK_CUR) != fsize) {
		int a, b;
		a = ReadNextInt(f, fsize);
		b = ReadNextInt(f, fsize);
		this->dir.push_back((Direction)((a - 1) * 3 + b));
	}
	SDL_RWclose(f);

	if (this->block_cnt > this->dir.size())
		this->block_cnt = this->dir.size();

	this->next_time = 0;
	this->playing = false;
}

BlockCreator::~BlockCreator() {
	Mix_FreeMusic(this->music);
}

void BlockCreator::CreateBlock(vector<Block*> *blocks, bool auto_play) {
	if (!this->playing) {
		Mix_PlayMusic(music, 0);
		this->playing = true;
	}
	else if(this->next_time < this->pos.size()){
		int x = (this->pos[this->next_time].x + 1) * win_w / 8;
		int y = (this->pos[this->next_time].y + 1) * win_h / 6;
		//int x = win_w / 2;
		//int y = win_h / 2;
		if (this->dir[this->next_time] % 2 && this->dir[this->next_time] != 5) {
			blocks->push_back(new Block({ x,y }, 0, this->dir[this->next_time], auto_play));
		}
		else {
			blocks->push_back(new Block({ x,y }, 45, this->dir[this->next_time], auto_play));
		}
	}
	this->next_time++;
}

int BlockCreator::NextTime() {
	if (this->next_time >= this->data.size())
		return -1;
	return this->data[this->next_time];
}
