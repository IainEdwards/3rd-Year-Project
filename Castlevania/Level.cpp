#include "Level.h"
#include <fstream>

Level::Level()
{
	spawnPointX = 32;
	spawnPointY = 320;
}

Level::~Level()
{

}



void Level::LoadLevel(std::string levelID, TextureManager* tm, SDL_Renderer* renderer)
{
	this->levelName = levelID;

	std::string levelImage = levelID.append(".png");

	tm->load(levelImage, levelName, renderer);
	tm->load("player_idle.png", "player", renderer);

	tm->load("empty.png", "empty", renderer);

	setTiles(levelName);

	player.Reset(spawnPointX, spawnPointY);
}

void Level::UpdateLevel(SDL_Rect& camera)
{

	player.ApplyPhysics(levelTiles);

	player.setCamera(camera);


}

void Level::DrawLevel(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera)
{
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		tm->draw("empty", levelTiles[i]->Box().x - camera.x, levelTiles[i]->Box().y - camera.y, TILE_WIDTH, TILE_HEIGHT, renderer, SDL_FLIP_NONE);
	}

	tm->draw(levelName, 0 - camera.x, 0 - camera.y, LEVEL_WIDTH, LEVEL_HEIGHT, renderer, SDL_FLIP_NONE);

	tm->draw("player", player.PosX() - camera.x, player.PosY() - camera.y, 64, 64, renderer, SDL_FLIP_NONE);

}



bool Level::setTiles(std::string levelName)
{
	//Success flag
	bool tilesLoaded = true;

	//The tile offsets
	int x = 0, y = 80;

	//Open the map
	std::ifstream map(levelName.append(".map"));

	//Initialize the tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		//Determines what kind of tile will be made
		int tileType = -1;

		//Read tile from map file
		map >> tileType;

		//If the was a problem in reading the map
		if (map.fail())
		{
			//Stop loading map
			printf("Error loading map: Unexpected end of file!\n");
			tilesLoaded = false;
			break;
		}

		//If the number is a valid tile number
		if (tileType >= 0)
		{
			levelTiles[i] = new Tile(x, y, tileType);
		}

		//If we don't recognize the tile type
		else
		{
			//Stop loading map
			printf("Error loading map: Invalid tile type at %d!\n", i);
			tilesLoaded = false;
			break;
		}

		//Move to next tile spot
		x += TILE_WIDTH;

		//If we've gone too far
		if (x >= LEVEL_WIDTH)
		{
			//Move back
			x = 0;

			//Move to the next row
			y += TILE_HEIGHT;
		}
	}

	//Close the file
	map.close();

	//If the map was loaded fine
	return tilesLoaded;
}