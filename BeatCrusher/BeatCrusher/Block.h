#pragma once
#include <SDL.h>
#include <vector>
#include <SDL_mixer.h>
#include <string>
using namespace std;

#define particle_size 3
#define spark_size 200
#define block_target_point 250

enum Direction { LEFTUP = 1, LEFT, LEFTDOWN, UP, MID, DOWN, RIGHTUP, RIGHT, RIGHTDOWN };

class TextEffect {
private:
	static vector<string> str;
	static vector<int> effect;
	static vector<SDL_Texture*> tex;
	static vector<SDL_Point> pos;

public:
	static void Init();
	static void PushEffect(string s, SDL_Point p);
	static void Rendering(SDL_Renderer *renderer);
	static void Processing();
};

class Block {
private:
	bool crushed;
	double size;
	SDL_Point pos;
	SDL_Point cut_begin, cut_end;
	int degree;
	int destroy_level;
	int crushed_line;
	SDL_Point target_pos;
	int distance;
	int create_time;

	Direction dir;

	vector<SDL_Point> force;
	vector<int> particle;
	vector<SDL_Point> spark;

	bool perfect_crash_point;

	bool scored = false;

	void RenderBlock(SDL_Renderer *renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4, int color_mode, int alpha);
	void RenderBlockParticle(SDL_Renderer *renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4, int crushed_level, int lv = particle_size, int v = 0);

	bool CheckDirection(SDL_Point pos, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4);

	void ScoreCalc();

	int time;

	bool auto_crush;

public:
	Block(SDL_Point pos, int degree, Direction dir, bool auto_crush);
	~Block();
	bool CheckCrushed();
	void ChangeSize(int add);
	void ChangePos(SDL_Point pos);
	void Rotate(int degree);
	int GetDegree();
	void Rendering(SDL_Renderer *renderer);
	bool Processing(bool check, bool process);
	bool destroyed;
	void Move();
	bool Able();
	void Delay(int delay);
	
	static Mix_Chunk *crush_eff[3];
	static SDL_Point mouse_pos;
	static SDL_Point prev_mouse_pos;
};