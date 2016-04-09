#ifndef SUBWEAPON_H
#define SUBWEAPON_H

#include <list>

#include <SDL.h>

#include "TextureManager.h"
#include "Enemy.h"


enum SubWeaponType
{
	EMPTY,
	DAGGER,
	AXE,
	HOLY_WATER,
	CROSS,
	STOPWATCH

};

class SubWeapon
{
public:
	SubWeapon();
	~SubWeapon();

	void setSubWeapon(int x, int y, int directionValue, SubWeaponType type);

	void ApplyPhysics(Tile *tiles[], int totalTiles);

	void DrawSubWeapon(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	SDL_Rect SubWeaponBox();

	SubWeaponType Type();

	int DamageValue();

	bool checkCollision(SDL_Rect a, SDL_Rect b);

	void SetCooldown(int value);
	int Cooldown();

	int FlameTimer();
	int SFXTimer();

	void HolyWaterPhysics(Tile *tiles[], int totalTiles);
	void AxePhysics();
	void CrossPhysics();

	int CrossTimer();

private:

	SDL_Rect subWeaponBox;

	int direction;

	int hitCooldown;

	int damageValue;

	SubWeaponType subWeaponType;

	int frameTimer, frameCounter;

	float velX, velY;
	float posX, posY;

	//for when holy water lands
	bool flame;
	int flameTimer;

	//for axe sfx
	int sfxTimer;

	//for cross distance
	int crossTimer;
	float velChange;

};


#endif