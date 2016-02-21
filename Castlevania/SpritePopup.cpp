#include "SpritePopup.h"

SpritePopup::SpritePopup(){}

SpritePopup::~SpritePopup(){}

SDL_Rect SpritePopup::PopupBox()
{
	return popupBox;
}

PopupType SpritePopup::Type()
{
	return popupType;
}

int SpritePopup::Timer()
{
	return timer;
}

void SpritePopup::setPopup(int x, int y, PopupType type)
{
	popupBox.x = x;
	popupBox.y = y;
	popupBox.w = 16;
	popupBox.h = 32;

	popupType = type;

	timer = 0;
}

void SpritePopup::ApplyPhysics()
{
	timer++;
}

void SpritePopup::DrawPopup(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	switch (popupType)
	{
	case HIT:
		tm->draw("hit", popupBox.x - camera.x, popupBox.y - camera.y, popupBox.w, popupBox.h, renderer, SDL_FLIP_NONE);
		break;

	case DEATH:
		tm->drawFrame("death", popupBox.x - camera.x, popupBox.y - camera.y, popupBox.w, popupBox.h, 1, timer / 8, renderer, SDL_FLIP_NONE);
		break;

	default:
		break;
	}
}

