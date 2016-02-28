#ifndef SUBWEAPON_H
#define SUBWEAPON_H

#include <SDL.h>
#include <list>
#include "TextureManager.h"
#include "Enemy.h"


enum SubWeaponType
{
	EMPTY,
	DAGGER,
	THROWING_AXE,
	HOLY_WATER,
	BOOMERANG,
	STOPWATCH

};

class SubWeapon
{
public:
	SubWeapon();
	~SubWeapon();

	void setSubWeapon(int x, int y, int directionValue, SubWeaponType type);

	void ApplyPhysics();

	void DrawSubWeapon(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	SDL_Rect SubWeaponBox();

	SubWeaponType Type();

	void SetCooldown(int value);
	int Cooldown();

private:

	SDL_Rect subWeaponBox;

	int direction;

	int hitCooldown;

	SubWeaponType subWeaponType;
};


#endif