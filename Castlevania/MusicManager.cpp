#include "MusicManager.h"

std::map<std::string, Mix_Music*> musicMap;

//Load the music into the music map
bool MusicManager::load(std::string fileName, std::string id)
{
	Mix_Music* tempMusic = Mix_LoadMUS(fileName.c_str());

	if (tempMusic != 0)
	{
		musicMap[id] = tempMusic;
		return true;
	}

	return false;
}

//Play the music, specify number of loops
void MusicManager::play(std::string id, int loops)
{
	Mix_PlayMusic(musicMap[id], loops);	
}

//Set Volume between 0 and 128
void MusicManager::setVolume(int volume)
{
	Mix_VolumeMusic(volume);
}