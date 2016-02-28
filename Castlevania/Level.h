#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <list>
#include <vector>
#include "Tile.h"
#include "Player.h"
#include "Enemy.h"
#include "DestroyableObject.h"
#include "Pickup.h"
#include "SpritePopup.h"
#include "SubWeapon.h"
#include "SoundManager.h"

class Level
{
public:

	Level();
	~Level();	

	void LoadLevel(std::string levelID, int levelN, TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm);

	void UpdateLevel(SDL_Rect& camera, SoundManager* sm);

	void DrawLevel(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	void DrawLevelChange(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount, int doorTimer);

	bool setTiles(std::string levelName);

	void setPickups(std::string levelName);

	Player player;

	std::list<Enemy> enemies;

	std::list<DestroyableObject> objects;

	std::list<Pickup> pickups;	

	void SpawnPickup(int x, int y, int offset);

	std::list <SpritePopup> popups;

	void PopupSprite(int x, int y, PopupType type);

	std::list <SubWeapon> projectiles;

	void SpawnProjectile(int x, int y, int directionValue, SubWeaponType type);

	//Tile constants
	const int TILE_WIDTH = 32;
	const int TILE_HEIGHT = 32;

	Tile* levelTiles[5000];

	char pickupTiles[5000];

	bool checkCollision(SDL_Rect a, SDL_Rect b);	

	int Score();

	void SpawnEnemies(SDL_Rect& camera);	

	void SetPreviousLevelSpawn(bool spawnOne, bool spawnTwo);

	void SetNextLevelSpawn(bool spawnTwo);


private:

	int levelWidth;	
	int levelHeight;
	int totalTiles;

	std::string levelName;	

	int spawnPointX, spawnPointY;
	int spawnPoint2X, spawnPoint2Y;
	int spawnPoint3X, spawnPoint3Y;

	int spawnCooldown;

	int score;

	bool pause;
	int pauseTimer;

	bool previousLevel, previousLevel2;
	bool secondExit;
};

#endif