#include "Pickup.h"

Pickup::Pickup(){}

Pickup::~Pickup(){}

SDL_Rect Pickup::PickupBox()
{
	return pickupBox;
}
PickupType Pickup::Type()
{
	return pickupType;
}

void Pickup::setPickup(int x, int y, PickupType type)
{
	pickupBox.x = x;
	pickupBox.y = y;
	pickupBox.w = 32;
	pickupBox.h = 32;

	startX = x;
	offsetX = 16;

	pickupType = type;

	velY = 0.0f;
	velX = 0.0f;

	right = true;
	stop = false;
}

void Pickup::ApplyPhysics(Tile *tiles[], int totalTiles)
{	
	if (!stop)
	{
		velY += 0.5f;

		switch (pickupType)
		{
		case WHIP:
			pickupBox.y += (int)round(velY);
			break;
		case SMALL_HEART:
			if (!stop)
				velY = 1.0f;
			if (right && !stop)
				velX += 0.2f;
			if (!right && !stop)
				velX -= 0.2f;
			if (pickupBox.x >= startX + offsetX)
				right = false;
			if (pickupBox.x <= startX && !right)
			{
				right = true;
				offsetX = 0;
			}
				
			pickupBox.y += (int)round(velY);
			pickupBox.x += (int)round(velX);
			break;
		case BIG_HEART:
			pickupBox.y += (int)round(velY);
			break;
		case RED_MONEYBAG:
			pickupBox.y += (int)round(velY);
			break;
		case PURPLE_MONEYBAG:
			pickupBox.y += (int)round(velY);
			break;
		case WHITE_MONEYBAG:
			pickupBox.y += (int)round(velY);
			break;
		case DAGGER_DROP:
			pickupBox.y += (int)round(velY);
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() != '0')
		{
			if (checkCollision(pickupBox, tiles[i]->Box()) && !stop)
			{
				pickupBox.y -= (int)round(velY) - 2;
				velY = 0.0f;
				velX = 0.0f;
				stop = true;
			}
		}
	}



}

void Pickup::DrawPickup(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera)
{
	switch (pickupType)
	{
	case WHIP:
		tm->draw("whip", pickupBox.x - camera.x, pickupBox.y - camera.y, pickupBox.w, pickupBox.h, renderer, SDL_FLIP_NONE);
		break;
	case SMALL_HEART:
		tm->draw("small_heart", pickupBox.x - camera.x, pickupBox.y - camera.y, pickupBox.w, pickupBox.h, renderer, SDL_FLIP_NONE);
		break;
	case BIG_HEART:
		tm->draw("big_heart", pickupBox.x - camera.x, pickupBox.y - camera.y, pickupBox.w, pickupBox.h, renderer, SDL_FLIP_NONE);
		break;
	case RED_MONEYBAG:
		tm->draw("red_moneybag", pickupBox.x - camera.x, pickupBox.y - camera.y, pickupBox.w, pickupBox.h, renderer, SDL_FLIP_NONE);
		break;
	case PURPLE_MONEYBAG:
		tm->draw("purple_moneybag", pickupBox.x - camera.x, pickupBox.y - camera.y, pickupBox.w, pickupBox.h, renderer, SDL_FLIP_NONE);
		break;
	case WHITE_MONEYBAG:
		tm->draw("white_moneybag", pickupBox.x - camera.x, pickupBox.y - camera.y, pickupBox.w, pickupBox.h, renderer, SDL_FLIP_NONE);
		break;
	case DAGGER_DROP:
		tm->draw("dagger", pickupBox.x - camera.x, pickupBox.y - camera.y, pickupBox.w, pickupBox.h, renderer, SDL_FLIP_NONE);
		break;

	default:
		break;
	}
	
}

bool Pickup::checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}