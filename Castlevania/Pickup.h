#ifndef PICKUP_H
#define PICKUP_H

#include <SDL.h>
#include "Tile.h"
#include "TextureManager.h"

enum PickupType
{
	AXE,
	CROSS,
	HOLY_WATER,
	KNIFE,
	STOPWATCH,
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

	void DrawPickup(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera);

	SDL_Rect PickupBox();

	PickupType Type();

	bool checkCollision(SDL_Rect a, SDL_Rect b);

private:

	PickupType pickupType;

	SDL_Rect pickupBox;

	float velY, velX;
	int startX, offsetX;

	bool right, stop;



};


#endif