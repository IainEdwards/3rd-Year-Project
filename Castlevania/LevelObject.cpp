#include "LevelObject.h"

LevelObject::LevelObject(){}

LevelObject::~LevelObject(){}

LevelObjectType LevelObject::Type()
{
	return levelObjectType;
}

SDL_Rect LevelObject::LevelObjectBox()
{
	return levelObjectBox;
}

bool LevelObject::Flip()
{
	return flip;
}


void LevelObject::setLevelObject(int x, int y, LevelObjectType type)
{
	levelObjectBox.x = x;
	levelObjectBox.y = y;

	posX = (float)x;
	velX = 0.0f;

	levelObjectType = type;

	flip = false;

	switch (levelObjectType)
	{
	case PLATFORM:
		levelObjectBox.w = 64;
		levelObjectBox.h = 20;
		break;

	case CRUSHER_1:
		crushTimer = 0;
		levelObjectBox.w = 64;
		levelObjectBox.h = 20;
		break;

	case CRUSHER_2:
		crushTimer = 0;
		levelObjectBox.w = 64;
		levelObjectBox.h = 20;
		break;

	default:
		break;
	}
}

void LevelObject::ApplyPhysics(Tile* tiles[], int totalTiles)
{
	switch (levelObjectType)
	{
	case PLATFORM:
		if (flip)
			velX = -1.0f;
		if (!flip)
			velX = 1.0f;

		posX += velX;
		levelObjectBox.x = (int)round(posX);
		if (touchesWall(tiles, totalTiles))
		{
			posX -= velX;

			flip = !flip;
		}
		levelObjectBox.x = (int)round(posX);
		break;

	case CRUSHER_1:
		crushTimer++;
		if (crushTimer < 85)
		{
			if (crushTimer % 12 == 0)
			{
				levelObjectBox.y -= 16;
			}
		}
		if (crushTimer >= 91 && crushTimer < 133)
		{
			if (crushTimer % 6 == 0)
			{
				levelObjectBox.y += 16;
			}
		}
		if (crushTimer >= 133)
			crushTimer = 0;
		break;

	case CRUSHER_2:
		crushTimer++;
		if (crushTimer < 49)
		{
			if (crushTimer % 12 == 0)
			{
				levelObjectBox.y -= 16;
			}
		}
		if (crushTimer >= 55 && crushTimer < 79)
		{
			if (crushTimer % 6 == 0)
			{
				levelObjectBox.y += 16;
			}
		}
		if (crushTimer >= 79)
			crushTimer = 0;
		break;

	default:
		break;
	}
}

void LevelObject::DrawLevelObject(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera)
{
	switch (levelObjectType)
	{
	case PLATFORM:
		tm->draw("platform", levelObjectBox.x - camera.x, levelObjectBox.y - camera.y, levelObjectBox.w, levelObjectBox.h, renderer, SDL_FLIP_NONE);
		break;

	case CRUSHER_1:
		tm->draw("crusher", levelObjectBox.x - camera.x, levelObjectBox.y - 124 - camera.y, levelObjectBox.w, 144, renderer, SDL_FLIP_NONE);
		break;

	case CRUSHER_2:
		tm->draw("crusher2", levelObjectBox.x - camera.x, levelObjectBox.y - 76 - camera.y, levelObjectBox.w, 96, renderer, SDL_FLIP_NONE);
		break;

	default:
		break;
	}
}

bool LevelObject::checkCollision(SDL_Rect a, SDL_Rect b)
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

bool LevelObject::touchesWall(Tile* tiles[], int totalTiles)
{
	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == '1')
		{
			if (checkCollision(levelObjectBox, tiles[i]->Box()))
			{
				return true;
			}
		}	
		if (tiles[i]->Type() == '2')
		{
			if (checkCollision(levelObjectBox, tiles[i]->Box()))
			{
				return true;
			}
		}

	}
	return false;
}