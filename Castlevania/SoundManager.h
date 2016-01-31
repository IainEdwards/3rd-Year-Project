#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <stdio.h>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
#include <map>

class SoundManager
{
public:
	bool load(std::string fileName, std::string id);	

	void play(std::string id);

	void setVolume(std::string id, int volume);

	std::map<std::string, Mix_Chunk*> soundMap;




};

#endif