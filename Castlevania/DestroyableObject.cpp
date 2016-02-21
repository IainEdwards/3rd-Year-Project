#include "DestroyableObject.h"

DestroyableObject::DestroyableObject(){}

DestroyableObject::~DestroyableObject(){}

ObjectType DestroyableObject::Type()
{
	return objectType;
}

void DestroyableObject::setObject(int x, int y, ObjectType type)
{
	objectBox.x = x;
	objectBox.y = y;

	objectType = type;

	switch (objectType)
	{
	case TORCH:
		objectBox.w = 32;
		objectBox.h = 64;
		break;
	case CANDLE:
		objectBox.w = 16;
		objectBox.h = 32;
		break;

	default:
		break;
	}
}

void DestroyableObject::DrawObject(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	switch (objectType)
	{
	case TORCH:
		tm->drawFrame("torch", objectBox.x - camera.x, objectBox.y - camera.y + 2, 32, 64, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		break;
	case CANDLE:
		tm->drawFrame("candle", objectBox.x - camera.x - 2, objectBox.y - camera.y + 2, 16, 32, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		break;
	default:
		break;
	}
}

SDL_Rect DestroyableObject::ObjectBox()
{
	return objectBox;
}