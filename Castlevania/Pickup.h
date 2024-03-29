#ifndef PICKUP_H
#define PICKUP_H

#include <SDL.h>
#include "Tile.h"
#include "TextureManager.h"

enum PickupType
{
	AXE_DROP,
	CROSS_DROP,
	HOLY_WATER_DROP,
	DAGGER_DROP,
	STOPWATCH_DROP,
	WHIP,
	SMALL_HEART,
	BIG_HEART,
	RED_MONEYBAG,
	PURPLE_MONEYBAG,
	WHITE_MONEYBAG,
	BONUS_MONEYBAG,
	ROAST,
	ROSARY,
	INVINCIBILITY,
	DOUBLE_SHOT,
	TRIPLE_SHOT,
	SPIRIT_BALL

};

class Pickup
{
public:
	Pickup();
	~Pickup();

	void setPickup(int x, int y, PickupType type);

	void ApplyPhysics(Tile *tiles[], int totalTiles);

	void DrawPickup(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	SDL_Rect PickupBox();

	PickupType Type();

	bool checkCollision(SDL_Rect a, SDL_Rect b);

	int PickupTimer();

private:

	PickupType pickupType;

	SDL_Rect pickupBox;

	float velY, velX;
	int startX, offsetX;

	bool right, stop;

	int pickupTimer;



};


#endif