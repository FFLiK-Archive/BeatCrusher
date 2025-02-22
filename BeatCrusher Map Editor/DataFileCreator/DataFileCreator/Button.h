#pragma once
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
using namespace std;
class Button
{
private:
	SDL_Color color;
	SDL_Color color_text;
	SDL_Rect rect, text_rect;
	SDL_Texture *text;
	SDL_Renderer *renderer;
	bool select;
	bool click;

public:
	void MakeButton(SDL_Renderer *renderer, string str, int x, int y, int w, int h, int fontsize, int r, int g, int b, int tr = 255, int tg = 255, int tb = 255);
	void MakeButtonTexture(string adr, int x, int y, int w, int h);
	void ChangeSize(int w, int h);
	void Move(int x, int y);
	void Rendering();
	bool Click(SDL_Event event);
	void Select(bool select);
	void Destroy();
};

