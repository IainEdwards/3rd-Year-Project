#include "SoundManager.h"

std::map<std::string, Mix_Chunk*> soundMap;

//Load the sound into the sound map
bool SoundManager::load(std::string fileName, std::string id)
{
	Mix_Chunk* tempSound = Mix_LoadWAV(fileName.c_str());

	if (tempSound != 0)
	{
		soundMap[id] = tempSound;
		return true;
	}

	return false;
}

//Play the sound
void SoundManager::play(std::string id)
{
	Mix_PlayChannel(-1, soundMap[id], 0);
}

//Set Volume between 0 and 128 for specific sound
void SoundManager::setVolume(std::string id, int volume)
{
	Mix_VolumeChunk(soundMap[id], 32);
}