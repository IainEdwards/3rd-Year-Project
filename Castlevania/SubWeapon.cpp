#include "SubWeapon.h"

SubWeapon::SubWeapon(){}

SubWeapon::~SubWeapon(){}

SDL_Rect SubWeapon::SubWeaponBox()
{
	return subWeaponBox;
}

SubWeaponType SubWeapon::Type()
{
	return subWeaponType;
}

void SubWeapon::SetCooldown(int value)
{
	hitCooldown = value;
}

int SubWeapon::Cooldown()
{
	return hitCooldown;
}

void SubWeapon::setSubWeapon(int x, int y, int directionValue, SubWeaponType type)
{
	subWeaponBox.x = x;
	subWeaponBox.y = y;
	subWeaponBox.w = 32;
	subWeaponBox.h = 32;

	subWeaponType = type;

	direction = directionValue;
}

void SubWeapon::ApplyPhysics()
{
	hitCooldown--;

	switch (subWeaponType)
	{
	case EMPTY:
		break;

	case DAGGER:
		subWeaponBox.x += 5 * direction;
		break;

	default:
		break;
	}
}

void SubWeapon::DrawSubWeapon(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	switch (subWeaponType)
	{
	case EMPTY:
		break;

	case DAGGER:
		if (direction == 1)
			tm->draw("dagger", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, renderer, SDL_FLIP_NONE);
		else if (direction == -1)
			tm->draw("dagger", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, renderer, SDL_FLIP_HORIZONTAL);
		break;

	default:
		break;
	}
}


