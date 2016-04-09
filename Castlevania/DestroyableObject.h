#ifndef DESTROYABLE_OBJECT_H
#define DESTROYABLE_OBJECT_H

#include <SDL.h>
#include "TextureManager.h"

enum ObjectType
{
	TORCH,
	CANDLE,
	TILE_1,
	TILE_2,
	TILE_3,
	TILE_4
};

class DestroyableObject
{
public:
	DestroyableObject();
	~DestroyableObject();

	void setObject(int x, int y, ObjectType type);

	void DrawObject(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	ObjectType Type();

	SDL_Rect ObjectBox();

private:

	SDL_Rect objectBox;

	ObjectType objectType;

};

#endif