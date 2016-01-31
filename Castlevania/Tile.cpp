#include "Tile.h"


Tile::Tile(int x, int y, char tileType)
{
	//Get the offsets
	tileBox.x = x;
	tileBox.y = y;

	//Set the collision box
	tileBox.w = TILE_WIDTH;
	tileBox.h = TILE_HEIGHT;

	//Get the tile type
	type = tileType;
}

//void Tile::RenderTile(SDL_Rect& camera, Tile* tiles[], TextureManager* tm, SDL_Renderer* renderer)
//{
//	//If the tile is on screen
//	if (checkCollision(camera, tileBox))
//	{
//		//Show the tile
//		//Texture.render(tileBox.x - camera.x, tileBox.y - camera.y, &gTileClips[mType]);
//
//
//		tm->draw("tiles", tileBox.x - camera.x, tileBox.y - camera.y, TILE_WIDTH, TILE_HEIGHT, renderer, SDL_FLIP_NONE);
//	}
//}

char Tile::Type()
{
	return type;
}

SDL_Rect Tile::Box()
{
	return tileBox;
}

bool Tile::checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

int Tile::Row()
{
	return tileRow;
}

int Tile::Frame()
{
	return tileFrame;
}