#ifndef SPRITE_POPUP_H
#define SPRITE_POPUP_H

#include <SDL.h>
#include "TextureManager.h"

enum PopupType
{
	HIT,
	DEATH,
	RED_POINTS,
	PURPLE_POINTS,
	WHITE_POINTS
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

	SDL_Rect popupBox;

	PopupType popupType;
};



#endif