#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>
#include <SDL_image.h>
#include "Tile.h"
#include "Player.h"

class Level
{
public:

	Level();
	~Level();	

	void LoadLevel(std::string levelID, TextureManager* tm, SDL_Renderer* renderer);

	void UpdateLevel(SDL_Rect& camera);

	void DrawLevel(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera);

	bool setTiles(std::string levelName);

	Player player;

	//Tile constants
	const int TILE_WIDTH = 32;
	const int TILE_HEIGHT = 32;
	const int TOTAL_TILES = 2112;	

	const int LEVEL_WIDTH = 5632;
	const int LEVEL_HEIGHT = 448;

	Tile* levelTiles[2112];

private:

	std::string levelName;

	int spawnPointX;
	int spawnPointY;

	

	

};

#endif