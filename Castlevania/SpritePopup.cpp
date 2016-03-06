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

	posX = (float)x;
	posY = (float)y;
	velY = velX = 0.0f;

	switch (popupType)
	{
	case RED_POINTS:
		popupBox.w = 26;
		popupBox.h = 16;
		break;

	case PURPLE_POINTS:
		popupBox.w = 26;
		popupBox.h = 16;
		break;

	case WHITE_POINTS:
		popupBox.w = 26;
		popupBox.h = 16;
		break;

	case SPLASH_LEFT:
		velY = -4.0f;
		velX = -0.2f;
		break;

	case SPLASH_RIGHT:
		velY = -4.0f;
		velX = 0.2f;
		break;
	default:
		break;
	}
		

	
}

void SpritePopup::ApplyPhysics()
{
	timer++;

	switch (popupType)
	{
	case SPLASH_LEFT:
		velY += 0.3f;
		break;

	case SPLASH_RIGHT:
		velY += 0.3f;
		break;

	default:
		break;
	}
	posX += velX;
	posY += velY;
	popupBox.x = (int)round(posX);
	popupBox.y = (int)round(posY);

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

	case SPLASH_LEFT:		
		tm->draw("splash", popupBox.x - camera.x, popupBox.y - camera.y, popupBox.w, popupBox.h, renderer, SDL_FLIP_NONE);		
		break;

	case SPLASH_RIGHT:
		tm->draw("splash", popupBox.x - camera.x, popupBox.y - camera.y, popupBox.w, popupBox.h, renderer, SDL_FLIP_NONE);
		break;

	case RED_POINTS:
		tm->draw("100", popupBox.x - camera.x, popupBox.y - camera.y, popupBox.w, popupBox.h, renderer, SDL_FLIP_NONE);
		break;

	case PURPLE_POINTS:
		tm->draw("400", popupBox.x - camera.x, popupBox.y - camera.y, popupBox.w, popupBox.h, renderer, SDL_FLIP_NONE);
		break;

	case WHITE_POINTS:
		tm->draw("700", popupBox.x - camera.x, popupBox.y - camera.y, popupBox.w, popupBox.h, renderer, SDL_FLIP_NONE);
		break;

	default:
		break;
	}
}

