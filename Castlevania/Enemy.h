#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <string>
#include <vector>
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

	void ApplyPhysics();

	void DrawEnemy(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	int PosX();
	int PosY();

	bool Flip();

	void setEnemy(int x, int y, EnemyType type);

	SDL_Rect EnemyBox();

	int HitPoints();
	void takeHit();

	int getCooldown();
	void beginCooldown();

private:

	SDL_Rect enemyBox;

	EnemyType enemyType;

	bool flip;
	
	float velX;
	int hitPoints;

	int cooldown;

};

#endif