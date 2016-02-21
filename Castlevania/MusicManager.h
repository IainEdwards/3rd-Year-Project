#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#include <stdio.h>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
#include <map>

class MusicManager
{
public:
	bool load(std::string fileName, std::string id);

	void play(std::string id, int loops);

	void setVolume(int volume);

	std::map<std::string, Mix_Music*> musicMap;
};

#endif