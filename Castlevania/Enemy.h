#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include "TextureManager.h"
#include "Tile.h"

enum EnemyType
{
	AXE_KNIGHT,
	BAT,
	BAT_BLUE,
	BONE_DRAGON,
	BONE_TOWER,
	EAGLE,
	FLEAMAN,
	GHOST,
	FISHMAN,
	GHOUL,
	MEDUSA_HEAD,
	PANTHER,
	RAVEN,
	RED_SKELETON,
	SKELETON,
	SPEAR_KNIGHT,
	SNAKE,
	PROJECTILE,
	PROJECTILE_FAST
};

class Enemy
{
public:
	Enemy();	
	~Enemy();

	void ApplyPhysics(Tile *tiles[], int totalTiles, SDL_Rect playerBox, bool stopwatch);

	void DrawEnemy(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	int PosX();
	int PosY();

	EnemyType Type();

	bool Flip();

	void setEnemy(int x, int y, EnemyType type, bool flip);

	bool checkCollision(SDL_Rect a, SDL_Rect b);

	bool touchesWall(Tile* tiles[], int totalTiles);

	SDL_Rect EnemyBox();

	float HitPoints();
	void takeHit(int whipLevel);

	int getCooldown();	

	void SetIdle(bool idleTo);
	bool Idle();

	void GhoulPhysics(Tile *tiles[], int totalTiles);
	void PantherPhysics(Tile *tiles[], int totalTiles, SDL_Rect playerBox);
	void BatPhysics(SDL_Rect playerBox);
	void FishmanPhysics(Tile *tiles[], int totalTiles);
	void SpearKnightPhysics(Tile *tiles[], int totalTiles, SDL_Rect playerBox);
	void MedusaHeadPhysics();
	void BoneTowerPhysics(SDL_Rect playerBox);
	bool Splash();
	bool DoShot();
	void ProjectilePhysics(Tile *tiles[], int totalTiles, float speed);

	bool DeleteEnemy();


private:

	SDL_Rect enemyBox;

	EnemyType enemyType;

	bool flip;
	bool idle;
	
	bool doJump;
	bool jumping;
	int jumpTimer;

	bool splash;

	bool doShot;
	bool shooting;
	int shootTimer;
	int shootCooldown;

	float velX, velY;
	float posX, posY;
	int startY, startX;
	float hitPoints;

	int cooldown;
	int enemyTimer;

	//for framecounting in 3s
	int frameTimer;
	int frameCounter;

	bool deleteEnemy;

};

#endif