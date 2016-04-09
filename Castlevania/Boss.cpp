#include "Boss.h"

Boss::Boss(){}

Boss::~Boss(){}

BossType Boss::Type()
{
	return bossType;
}

SDL_Rect Boss::BossBox()
{
	return bossBox;
}

int Boss::Health()
{
	return hitPoints;
}

void Boss::setAwake()
{
	awake = true;
}

bool Boss::Awake()
{
	return awake;
}

bool Boss::AwakeStart()
{
	return awakeStart;
}

bool Boss::SpawnMob()
{
	return spawnMob;
}

void Boss::setBoss(int x, int y, BossType type, int totalLevelTiles)
{
	bossBox.x = x;
	bossBox.y = y;

	posX = (float)x;
	posY = (float)y;

	startY = (float)y;

	velX = 0.0f;

	flip = 1;

	bossType = type;

	totalTiles = totalLevelTiles;

	awake = false;
	awakeStart = false;

	

	switch (bossType)
	{
	case VAMPIRE_BAT:
		batTimer = 0;
		swooping = false;
		drop = false;
		start = true;

		bossBox.w = 96;
		bossBox.h = 48;

		hitPoints = 16;

		break;

	case MEDUSA:
		bossBox.w = 64;
		bossBox.h = 64;

		hitPoints = 16;
		medusaTimer = 0;
		break;

	default:
		break;
	}
}

void Boss::ApplyPhysics(int playerPosX, int playerPosY, Tile* tiles[])
{
	if (cooldown > 0)
		cooldown--;

	if (cooldown <= 12)
	{
		switch (bossType)
		{
		case VAMPIRE_BAT:
			VampireBatPhysics(playerPosX, playerPosY, tiles);
			break;

		case MEDUSA:
			MedusaPhysics(playerPosX, playerPosY, tiles);
			break;

		default:
			break;
		}
	}
}

void Boss::DrawBoss(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	switch (bossType)
	{
	case VAMPIRE_BAT:
		if (!awake)
			tm->draw("vampire_bat_idle", bossBox.x - camera.x, bossBox.y - camera.y, bossBox.w, bossBox.h, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("vampire_bat", bossBox.x - camera.x, bossBox.y - camera.y, bossBox.w, bossBox.h, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		break;

	case MEDUSA:
		if (!awakeStart && medusaTimer < 60)
			tm->draw("medusa_idle", bossBox.x - camera.x, bossBox.y - camera.y, bossBox.w, bossBox.h, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("medusa", bossBox.x - camera.x, bossBox.y - camera.y, bossBox.w, bossBox.h, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		break;

	case FINISHED:
		break;
	default:
		break;
	}
}

void Boss::takeHit(int whipLevel)
{
	if (whipLevel >= 2)
		hitPoints -= 2;
	else
		hitPoints--;

	cooldown = 36;
}

int Boss::getCooldown()
{
	return cooldown;
}

bool Boss::touchesWall(SDL_Rect box, Tile* tiles[])
{
	//Go through the tiles
	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == '3')
		{
			if (checkCollision(box, tiles[i]->Box()))
			{
				return true;
			}
		}
		if (tiles[i]->Type() == 'E')
		{
			if (checkCollision(box, tiles[i]->Box()))
			{
				return true;
			}
		}
	}

	return false;
}

bool Boss::checkCollision(SDL_Rect a, SDL_Rect b)
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

void Boss::VampireBatPhysics(int playerPosX, int playerPosY, Tile* tiles[])
{
	batTimer++;

	if (batTimer > 60 && !awakeStart)
		awakeStart = true;

	if (batTimer == 0)
	{
		flip = 1;
		velX = 0.0f;
	}

	if (batTimer < 100 && start)
	{
		velX = -1.0f;
		posY += 1.5f;
	}
	else if (start)
	{
		velX = 0.0f;
	}
	else if (batTimer < 120 && !start)
	{
		if (posY < 272)
			posY += 0.5f;
		else if (posY > 272)
			posY -= 0.5f;

		velX = 0.0f;
	}

	if (batTimer > 120 && batTimer <= 240)
	{
		start = false;
		velX = 0.0f;
	}

	if (batTimer == 240)
	{

		targetX = (float)bossBox.x + ((playerPosX - (float)bossBox.x) * 2);
		targetY = playerPosY - posY + 24;
		attackX = (targetX - (float)bossBox.x) / 90;
		if (attackX > 4.0f)
			attackX = 4.0f;
		attackY = posY;
	}

	if (batTimer >= 240 && batTimer <= 285)
	{
		swooping = true;
		velX = attackX;
		posY = attackY + (targetY * sin((batTimer - 240) * 2 * 3.1415926f / 180.0f));
	}
	if (batTimer > 285 && batTimer <= 360)
	{
		velX = attackX;

		if (posY < 144)
			drop = true;

		if (!drop)
			posY = attackY + (targetY * sin((batTimer - 240) * 2 * 3.1415926f / 180.0f));
		else if (drop)
			posY += 2.0f;
	}

	if (batTimer > 360 && batTimer <= 480)
	{
		swooping = false;
		flip = 1;

		if (posX > playerPosX)
			velX = 1.0f;
		else if (posX < playerPosX)
			velX = -1.0f;

		if (posY < 272)
			posY += 0.5f;
		else if (posY > 272)
			posY -= 0.5f;

	}

	if (batTimer > 480)
	{
		flip = 1;
		drop = false;
		batTimer = 210;
	}

	bossBox.x += (int)round(velX * flip);
	bossBox.y = (int)round(posY);

	if (touchesWall(bossBox, tiles))
	{
		bossBox.x -= (int)round(velX * flip);
		velX = 0.0f;
		if (swooping && flip == 1)
			flip = -1;
		else if (swooping && flip == -1)
			flip = 1;
	}
}

void Boss::MedusaPhysics(int playerPosX, int playerPosY, Tile* tiles[])
{
	medusaTimer++;

	if (spawnMob == true)
		spawnMob = false;

	if (medusaTimer > 120 && !awakeStart)
		awakeStart = true;

	if (awakeStart)
	{
		medusaTimer2++;

		posY = startY + 32 - (32 * cos(medusaTimer2 * 3 * 3.1415926f / 180.0f));

		bossBox.y = (int)round(posY);
		
		velX = 1.8f * (float)flip;		

		posX += velX;
		bossBox.x = (int)round(posX);

		if (touchesWall(bossBox, tiles))
		{
			posX -= velX;
			flip = -flip;
		}
		bossBox.x = (int)round(posX);

		if (medusaTimer2 == 60 || medusaTimer2 == 240)
		{
			spawnMob = true;
			awakeStart = false;			
			medusaTimer = 90;			
		}

		if (medusaTimer2 == 180)
		{
			spawnMob = true;
			awakeStart = false;
			medusaTimer = 90;

			if (playerPosX < bossBox.x)
				flip = -1;
			else if (playerPosX > bossBox.x)
				flip = 1;
		}

		if (medusaTimer2 > 360)
		{
			if (playerPosX < bossBox.x)
				flip = -1;
			else if (playerPosX > bossBox.x)
				flip = 1;

			spawnMob = true;
			awakeStart = false;
			medusaTimer = 60;	
			medusaTimer2 = 0;
		}

		
	}

	
}