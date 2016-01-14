#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL.h>
#include <SDL_image.h>
#include "Texture2D.h"
#include <string>

class Animation
{
public:

	Animation(Texture2D texture, float frameTime, bool isLooping);
	//Animation(Texture2D texture, float frameTime, bool isLooping);
	~Animation();

	Texture2D getTexture();
	float getFrameTime();
	bool getIsLooping();
	int getFrameCount();
	int getFrameWidth();
	int getFrameHeight();

private:
	Texture2D Texture;
	float FrameTime;
	bool IsLooping;
	int FrameCount;
	int FrameWidth;
	int FrameHeight;

};
#endif