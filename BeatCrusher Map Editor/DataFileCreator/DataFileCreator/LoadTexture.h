#pragma once
#include <vector>
#include <string>
using namespace std;
#include <SDL.h>
SDL_Texture* LoadImage(const char* filename, SDL_Renderer *renderer);
SDL_Texture * LoadText(const char * str, SDL_Renderer * renderer, int size, const char *fontfile_name, int r = 255, int g = 255, int b = 255);
SDL_Texture * LoadText(const wstring str, SDL_Renderer * renderer, int size, const char *fontfile_name);