#ifndef DESTROYABLE_OBJECT_H
#define DESTROYABLE_OBJECT_H

#include <SDL.h>
#include "TextureManager.h"

enum ObjectType
{
	TORCH,
	CANDLE
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