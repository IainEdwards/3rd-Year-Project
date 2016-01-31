#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <list>
#include "Tile.h"
#include "Player.h"
#include "Enemy.h"
#include "SoundManager.h"

class Level
{
public:

	Level();
	~Level();	

	void LoadLevel(std::string levelID, int levelN, TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm);

	void ClearLevel();

	void UpdateLevel(SDL_Rect& camera);

	void DrawLevel(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	bool setTiles(std::string levelName);

	Player player;

	Enemy enemy, enemy2, enemy3, enemy4, enemy5;

	//std::vector<Enemy> enemies;

	std::list<Enemy> enemies;

	//Tile constants
	const int TILE_WIDTH = 32;
	const int TILE_HEIGHT = 32;

	Tile* levelTiles[5000];

	bool checkCollision(SDL_Rect a, SDL_Rect b);	

	bool ReachedExit();

	int Score();

	void SpawnEnemies(SDL_Rect& camera);

private:

	int levelWidth;	
	int levelHeight;
	int totalTiles;

	std::string levelName;	

	int spawnPointX;
	int spawnPointY;

	int spawnCooldown;

	int score;
};

#endif