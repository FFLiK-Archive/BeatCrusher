#pragma once
#include "Info.h"
#include <SDL.h>
#include <string>
enum FILE_MODE {RES, DAT};
void SetDataFile();
std::string Location(FILE_MODE f, std::string pos, std::string name);

int ReadNextInt(SDL_RWops *file, int size);
std::string ReadNextLine(SDL_RWops *file, int size);
float ReadNextFloat(SDL_RWops *file, int size);
int GetSize(std::string file_name);

void WriteInt(SDL_RWops *file, int data);