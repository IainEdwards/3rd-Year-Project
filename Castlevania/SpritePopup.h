#ifndef SPRITE_POPUP_H
#define SPRITE_POPUP_H

#include <SDL.h>
#include "TextureManager.h"

enum PopupType
{
	HIT,
	DEATH,
	SPLASH_LEFT,
	SPLASH_RIGHT,
	RED_POINTS,
	PURPLE_POINTS,
	WHITE_POINTS,
	DEBRIS_LEFT,
	DEBRIS_RIGHT,
	DEBRIS_UP
};

class SpritePopup
{
public:
	SpritePopup();
	~SpritePopup();

	void setPopup(int x, int y, PopupType type);

	void ApplyPhysics();

	void DrawPopup(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	

	SDL_Rect PopupBox();

	PopupType Type();

	int Timer();


private:

	int timer;	

	float velX, velY;
	float posX, posY;

	SDL_Rect popupBox;

	PopupType popupType;
};



#endif