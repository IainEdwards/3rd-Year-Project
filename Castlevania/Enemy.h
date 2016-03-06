#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include "TextureManager.h"
#include "Tile.h"

enum EnemyType
{
	AXE_KNIGHT,
	BAT,
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
	SPEAR_KNIGHT
};

class Enemy
{
public:
	Enemy();	
	~Enemy();

	void ApplyPhysics(Tile *tiles[], int totalTiles);

	void DrawEnemy(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	int PosX();
	int PosY();

	EnemyType Type();

	bool Flip();

	void setEnemy(int x, int y, EnemyType type);

	bool checkCollision(SDL_Rect a, SDL_Rect b);

	bool touchesWall(SDL_Rect box, Tile* tiles[], int totalTiles);

	SDL_Rect EnemyBox();

	int HitPoints();
	void takeHit(int whipLevel);

	int getCooldown();	

	void SetIdle(bool idleTo);

	void GhoulPhysics(Tile *tiles[], int totalTiles);
	void PantherPhysics(Tile *tiles[], int totalTiles);

private:

	SDL_Rect enemyBox;

	EnemyType enemyType;

	bool flip;
	bool idle;
	
	bool doJump;
	bool jumping;
	int jumpTimer;

	float velX, velY;
	float posX, posY;
	int hitPoints;

	int cooldown;

	//for framecounting in 3s
	int frameTimer;
	int frameCounter;

};

#endif