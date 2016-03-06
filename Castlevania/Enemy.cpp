#include "Enemy.h"

Enemy::Enemy(){}

Enemy::~Enemy(){}

int Enemy::PosX()
{
	return enemyBox.x;
}

int Enemy::PosY()
{
	return enemyBox.y;
}

EnemyType Enemy::Type()
{
	return enemyType;
}

void Enemy::SetIdle(bool idleTo)
{
	idle = idleTo;
}

void Enemy::setEnemy(int x, int y, EnemyType type)
{
	enemyBox.x = x;
	enemyBox.y = y;
	posX = (float)x;
	posY = (float)y;

	velX = velY = 0.0f;

	enemyType = type;

	switch (enemyType)
	{
	case GHOUL:
		flip = true;		
		enemyBox.w = 32;
		enemyBox.h = 64;
		hitPoints = 1;
		cooldown = 0;
		break;

	case PANTHER:
		idle = true;
		flip = true;
		doJump = false;
		jumping = false;
		jumpTimer = 0;
		enemyBox.w = 64;
		enemyBox.h = 32;
		hitPoints = 1;
		cooldown = 0;
		frameTimer = 0;
		frameCounter = 0;
		break;

	default:
		break;
	}	
}

void Enemy::ApplyPhysics(Tile *tiles[], int totalTiles)
{
	cooldown--;
	switch (enemyType)
	{
	case GHOUL:
		GhoulPhysics(tiles, totalTiles);
		break;

	case PANTHER:
		PantherPhysics(tiles, totalTiles);
		break;

	default:
		break;
	}	

	
}

void Enemy::DrawEnemy(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	switch (enemyType)
	{
	case GHOUL:
		if (!flip)
			tm->drawFrame("ghoul", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 32, 64, 1, frameCount, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("ghoul", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 32, 64, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case PANTHER:
		if (idle)
		{
			if (!flip)
				tm->draw("panther_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 64, 32, renderer, SDL_FLIP_NONE);
			else
				tm->draw("panther_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 64, 32, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (jumping)
		{
			if (!flip)
				tm->draw("panther_jumping", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 64, 32, renderer, SDL_FLIP_NONE);
			else
				tm->draw("panther_jumping", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 64, 32, renderer, SDL_FLIP_HORIZONTAL);
		}
		else
		{
			

			if (!flip)
				tm->drawFrame("panther_running", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 64, 32, 1, frameCounter, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("panther_running", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 64, 32, 1, frameCounter, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	default:
		break;
	}
	
}

bool Enemy::Flip()
{
	return flip;
}

SDL_Rect Enemy::EnemyBox()
{
	return enemyBox;
}

int Enemy::HitPoints()
{
	return hitPoints;
}

void Enemy::takeHit(int whipLevel)
{
	if (whipLevel == 1)
		hitPoints--;
	else
		hitPoints -= 2;

	cooldown = 24;
	
}

int Enemy::getCooldown()
{
	return cooldown;
}

bool Enemy::checkCollision(SDL_Rect a, SDL_Rect b)
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

bool Enemy::touchesWall(SDL_Rect box, Tile* tiles[], int totalTiles)
{
	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == '1' || tiles[i]->Type() == '2')
		{
			if (checkCollision(enemyBox, tiles[i]->Box()))
			{
				return true;
			}
		}

		if (tiles[i]->Type() == 'S' && enemyType != PANTHER)
		{
			if (checkCollision(enemyBox, tiles[i]->Box()))
			{
				return true;
			}
		}
	}
	
	return false;
}


void Enemy::GhoulPhysics(Tile *tiles[], int totalTiles)
{
	
	velY = 16.0f;

	if (flip)
		velX = -1.7f;
	if (!flip)
		velX = 1.7f;


	enemyBox.y +=(int)round(velY);
	posX += velX;

	if (touchesWall(enemyBox, tiles, totalTiles))
	{
		enemyBox.y -= (int)round(velY);
		velY = 0.0f;		
	}	

	enemyBox.x = (int)round(posX);
}

void Enemy::PantherPhysics(Tile *tiles[], int totalTiles)
{
	frameTimer++;
	if (60 / frameTimer <= 8)
	{
		frameTimer = 0;
		frameCounter++;
		if (frameCounter >= 3)
			frameCounter = 0;
	}

	if (idle)
	{
		velX = 0.0f;
		velY = 0.0f;
	}
	else
	{
		
		velY += 0.3f;

		if (!flip)
			velX = 3.6f;
		else
			velX = -3.6f;
		
		enemyBox.y += (int)round(velY);

		for (int i = 0; i < totalTiles; ++i)
		{
			if (tiles[i]->Type() == 'S')
			{
				if (checkCollision(enemyBox, tiles[i]->Box()) && !jumping)
				{
					doJump = true;
					enemyBox.y -= (int)round(velY);
				}
			}
		}

		if (doJump)
		{
			velY -= 5.5f;
			doJump = false;			
			jumping = true;			
		}

		if (jumping)
		{
			jumpTimer++;
			velX *= 1.2f;
		}
		posX += velX;
		enemyBox.x = (int)round(posX);

		if (touchesWall(enemyBox, tiles, totalTiles))
		{
			enemyBox.y -= (int)round(velY);
			velY = 0.0f;

			if (jumpTimer > 50)
			{
				if (flip)
					flip = false;
				else if (!flip)
					flip = true;

				jumpTimer = 0;
				jumping = false;
			}


			
			
		}

	}
}