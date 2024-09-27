#include "SetDataFile.h"
#include <iostream>
#include <string>
#include "SetDataFile.h"
using namespace std;

void SetDataFile() {
	FILE *f;
	int music_cnt;
	fopen_s(&f, Location(DAT, "std", "music.dat").c_str(), "r");
	fscanf_s(f, "%d", &music_cnt);
	fclose(f);

	for (int i = 1; i <= music_cnt; i++) {
		if (!fopen_s(&f, Location(DAT, to_string(i), "data.dat").c_str(), "r")) {
			fclose(f);
		}
		else {
			fopen_s(&f, Location(DAT, to_string(i), "data.dat").c_str(), "w");
			fprintf(f, "0 0");
			fclose(f);
		}

		if (!fopen_s(&f, Location(DAT, to_string(i), "rank.dat").c_str(), "r")) {
			fclose(f);
			remove(Location(DAT, to_string(i), "rank.dat").c_str());
		}
	}
}

std::string Location(FILE_MODE f, std::string pos, std::string name) {
	std::string loc;
	loc = name;

	if (pos != "") {
		loc = pos + "\\" + loc;
	}

	switch (f) {
	case RES:
		loc = "res\\" + loc;
		break;
	case DAT:
		loc = "dat\\" + loc;
		break;
	}

	if (RELEASE) {
		return (std::string)getenv("APPDATA") + "\\Beat Crusher\\" + loc;
	}
	else {
		return loc;
	}
}

int ReadNextInt(SDL_RWops * file, int size) {
	char data;
	string ret;
	while (1) {
		SDL_RWread(file, &data, sizeof(char), 1);
		if (data == ' ' || data == '\n')
			break;
		if (!(data >= '0' && data <= '9')) {
			continue;
		}
		ret += data;
		if (SDL_RWseek(file, 0, RW_SEEK_CUR) == size)
			break;
	}
	return stoi(ret);
}

std::string ReadNextLine(SDL_RWops * file, int size) {
	char data;
	string ret;
	while (1) {
		SDL_RWread(file, &data, sizeof(char), 1);
		if (data == '\n') {
			break;
		}
		ret += data;
		if (SDL_RWseek(file, 0, RW_SEEK_CUR) == size)
			break;
	}
	return ret;
}

float ReadNextFloat(SDL_RWops * file, int size) {
	char data;
	string ret;
	cout << "/";
	while (1) {
		SDL_RWread(file, &data, sizeof(char), 1);
		if (data == ' ' || data == '\n')
			break;
		if (!(data >= '0' && data <= '9' || data == '.')) {
			continue;
		}
		ret += data;
		if (SDL_RWseek(file, 0, RW_SEEK_CUR) == size)
			break;
	}
	return stof(ret);
}

int GetSize(std::string file_name) {
	SDL_RWops *f;
	f = SDL_RWFromFile(file_name.c_str(), "r");
	int ret = SDL_RWseek(f, 0, RW_SEEK_END);
	SDL_RWclose(f);
	return ret;
}

void WriteInt(SDL_RWops * file, int data) {
	string d = to_string(data);
	const char *c = d.c_str();
	SDL_RWwrite(file, c, sizeof(char), d.size());
	return;
}
