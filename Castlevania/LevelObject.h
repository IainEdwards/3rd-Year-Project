#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>
#include "TextureManager.h"
#include "Tile.h"

enum LevelObjectType
{
	PLATFORM,
	CRUSHER_1,
	CRUSHER_2
};

class LevelObject
{

public:
	LevelObject();
	~LevelObject();

	void setLevelObject(int x, int y, LevelObjectType type);

	void ApplyPhysics(Tile* tiles[], int totalTiles);

	void DrawLevelObject(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera);

	LevelObjectType Type();

	SDL_Rect LevelObjectBox();

	bool checkCollision(SDL_Rect a, SDL_Rect b);

	bool touchesWall(Tile* tiles[], int totalTiles);

	bool Flip();



private:

	LevelObjectType levelObjectType;

	SDL_Rect levelObjectBox;

	bool flip;

	float velX;
	float posX;

	int crushTimer;




};

#endif