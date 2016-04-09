#ifndef BOSS_H
#define BOSS_H

#include <SDL.h>
#include "TextureManager.h"
#include "Tile.h"

enum BossType
{
	VAMPIRE_BAT,
	MEDUSA,
	FINISHED
};

class Boss
{

public:
	Boss();
	~Boss();

	void ApplyPhysics(int playerPosX, int playerPosY, Tile* tiles[]);

	void DrawBoss(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	BossType Type();

	SDL_Rect BossBox();

	void setBoss(int x, int y, BossType type, int totalLevelTiles);

	bool touchesWall(SDL_Rect box, Tile* tiles[]);

	bool checkCollision(SDL_Rect a, SDL_Rect b);

	void takeHit(int whipLevel);

	int Health();

	int getCooldown();

	void setAwake();	

	bool Awake();

	bool AwakeStart();

	bool SpawnMob();

	void VampireBatPhysics(int playerPosX, int playerPosY, Tile* tiles[]);
	void MedusaPhysics(int playerPosX, int playerPosY, Tile* tiles[]);

private:

	BossType bossType;

	SDL_Rect bossBox;
	
	float posX, posY;
	float velX;

	int hitPoints;
	int cooldown;

	//Variables for vampire bat
	int batTimer;
	float targetX, targetY;
	float attackX, attackY;
	bool start;
	bool swooping, drop;

	int flip;

	bool awake;
	bool awakeStart;

	//Variables for medusa
	int medusaTimer, medusaTimer2;
	float startY;
	bool spawnMob;

	

	int totalTiles;

};


#endif