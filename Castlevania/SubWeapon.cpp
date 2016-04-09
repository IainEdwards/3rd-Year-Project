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

int SubWeapon::DamageValue()
{
	return damageValue;
}

void SubWeapon::SetCooldown(int value)
{
	hitCooldown = value;
}

int SubWeapon::Cooldown()
{
	return hitCooldown;
}

int SubWeapon::FlameTimer()
{
	return flameTimer;
}

int SubWeapon::SFXTimer()
{
	return sfxTimer;
}

int SubWeapon::CrossTimer()
{
	return crossTimer;
}

void SubWeapon::setSubWeapon(int x, int y, int directionValue, SubWeaponType type)
{
	subWeaponBox.x = x;
	subWeaponBox.y = y;
	subWeaponBox.w = 32;
	subWeaponBox.h = 32;

	subWeaponType = type;

	direction = directionValue;

	velX = velY = 0.0f;
	posX = (float)x;
	posY = (float)y;

	flame = false;
	flameTimer = 0;
	frameCounter = frameTimer = 0;
	sfxTimer = 0;
	crossTimer = 0;
	damageValue = 1;

	switch (subWeaponType)
	{
	case HOLY_WATER:
		velY = -2.0f;
		break;

	case AXE:
		velY = -9.0f;
		sfxTimer = 15;
		damageValue = 2;
		break;

	case CROSS:
		sfxTimer = 10;
		damageValue = 2;		
		velChange = 0.0f;
		break;

	default:
		break;
	}

	

	
	
}

void SubWeapon::ApplyPhysics(Tile *tiles[], int totalTiles)
{
	hitCooldown--;

	switch (subWeaponType)
	{
	case EMPTY:
		break;

	case DAGGER:
		subWeaponBox.x += 5 * direction;
		crossTimer++;
		break;

	case STOPWATCH:
		break;

	case HOLY_WATER:
		HolyWaterPhysics(tiles, totalTiles);
		break;

	case AXE:
		AxePhysics();
		break;

	case CROSS:
		CrossPhysics();
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

	case STOPWATCH:
		break;

	case HOLY_WATER:
		if (!flame)
		{
			if (direction == 1)
				tm->drawFrame("holy_water_flame", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, 1, 0, renderer, SDL_FLIP_NONE);
			else if (direction == -1)
				tm->drawFrame("holy_water_flame", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, 1, 0, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (flame)
		{
			tm->drawFrame("holy_water_flame", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, 1, frameCounter + 1, renderer, SDL_FLIP_NONE);
		}
		break;

	case AXE:

		if (direction == 1)
			tm->drawFrame("axe", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, 1, frameCounter, renderer, SDL_FLIP_NONE);
		else if (direction == -1)
			tm->drawFrame("axe", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, 1, frameCounter, renderer, SDL_FLIP_HORIZONTAL);
		break;
		

	case CROSS:
		if (direction == 1)
			tm->drawFrame("cross", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, 1, frameCounter, renderer, SDL_FLIP_NONE);
		else if (direction == -1)
			tm->drawFrame("cross", subWeaponBox.x - camera.x, subWeaponBox.y - camera.y, subWeaponBox.w, subWeaponBox.h, 1, frameCounter, renderer, SDL_FLIP_HORIZONTAL);
		break;
		break;

	default:
		break;
	}
}

bool SubWeapon::checkCollision(SDL_Rect a, SDL_Rect b)
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

void SubWeapon::HolyWaterPhysics(Tile *tiles[], int totalTiles)
{
	frameTimer++;
	if (60 / frameTimer <= 8)
	{
		frameTimer = 0;
		frameCounter++;
		if (frameCounter >= 5)
			frameCounter = 0;
	}

	if (!flame)
	{
		velX = 3.1f * (float)direction;
		velY += 0.2f;
		flameTimer = 0;
		frameTimer = 0;
	}
	else
	{
		velX = 0.0f;
		velY = 0.0f;
		flameTimer++;

	}

	posX += velX;
	subWeaponBox.x = (int)round(posX);

	posY += velY;
	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == '1' || tiles[i]->Type() == '2' || tiles[i]->Type() == 'S')
		{
			if (checkCollision(subWeaponBox, tiles[i]->Box()))
			{
				posY -= velY;
				velY = 0.0f;
				velX = 0.0f;
				flame = true;

			}
		}
	}

	subWeaponBox.y = (int)round(posY);
}

void SubWeapon::AxePhysics()
{
	frameTimer++;
	if (60 / frameTimer <= 10)
	{
		frameTimer = 0;
		frameCounter++;
		if (frameCounter >= 3)
			frameCounter = 0;
	}

	sfxTimer++;
	if (sfxTimer > 15)
		sfxTimer = 0;

	velX = 3.2f * (float)direction;
	velY += 0.32f;

	posX += velX;
	posY += velY;

	subWeaponBox.x = (int)round(posX);
	subWeaponBox.y = (int)round(posY);
}

void SubWeapon::CrossPhysics()
{	

	frameTimer++;
	if (60 / frameTimer <= 10)
	{
		frameTimer = 0;
		frameCounter++;
		if (frameCounter >= 3)
			frameCounter = 0;
	}

	sfxTimer++;
	if (sfxTimer > 10)
		sfxTimer = 0;

	crossTimer++;

	velX = (3.6f - velChange) * (float)direction;

	posX += velX;

	subWeaponBox.x = (int)round(posX);

	if (crossTimer > 76)
	{
		if (velChange < 3.6f)
			velChange += 0.6f;
	}
	if (crossTimer > 88)
	{
		if (velChange < 7.2f)
			velChange += 0.6f;
	}



}

