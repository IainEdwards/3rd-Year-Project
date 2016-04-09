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
	case TILE_1:
		objectBox.w = 32;
		objectBox.h = 32;
		break;
	case TILE_2:
		objectBox.w = 32;
		objectBox.h = 32;
		break;
	case TILE_3:
		objectBox.w = 32;
		objectBox.h = 32;
		break;
	case TILE_4:
		objectBox.w = 32;
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
		tm->drawFrame("torch", objectBox.x - camera.x, objectBox.y - camera.y + 2, objectBox.w, objectBox.h, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		break;
	case CANDLE:
		tm->drawFrame("candle", objectBox.x - camera.x - 2, objectBox.y - camera.y + 2, objectBox.w, objectBox.h, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		break;
	case TILE_1:
		tm->draw("tile1", objectBox.x - camera.x, objectBox.y - camera.y, objectBox.w, objectBox.h, renderer, SDL_FLIP_NONE);
		break;
	case TILE_2:
		tm->draw("tile2", objectBox.x - camera.x, objectBox.y - camera.y, objectBox.w, objectBox.h, renderer, SDL_FLIP_NONE);
		break;
	case TILE_3:
		tm->draw("tile3", objectBox.x - camera.x, objectBox.y - camera.y, objectBox.w, objectBox.h, renderer, SDL_FLIP_NONE);
		break;
	case TILE_4:
		tm->draw("tile4", objectBox.x - camera.x, objectBox.y - camera.y, objectBox.w, objectBox.h, renderer, SDL_FLIP_NONE);
		break;
	default:
		break;
	}
}

SDL_Rect DestroyableObject::ObjectBox()
{
	return objectBox;
}