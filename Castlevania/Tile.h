#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <SDL_image.h>
#include "TextureManager.h"

enum TileCollision
{
	//A passable tile is one which does not hinder motion at all
	Passable = 0,

	//An impassable tile is one which does not allow the player to move through it at all
	Impassable = 1,

	//A platform tile is one which is passable until the player is above it
	Platform = 2,
};

class Tile
{
public:
	//Initializes position and type
	Tile(int x, int y, int tileType);

	//Shows the tile
	void RenderTile(SDL_Rect& camera, Tile* tiles[], TextureManager* tm, SDL_Renderer* renderer);

	//Get the tile type
	int Type();

	int Row();

	int Frame();

	bool checkCollision(SDL_Rect a, SDL_Rect b);

	//Get the collision box
	SDL_Rect Box();

	const int TILE_WIDTH = 32;
	const int TILE_HEIGHT = 32;

private:
	//The attributes of the tile
	SDL_Rect tileBox;

	//The tile type
	int type;

	int tileRow;

	int tileFrame;
};



#endif