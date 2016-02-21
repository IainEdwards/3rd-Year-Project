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

void Enemy::setEnemy(int x, int y, EnemyType type)
{
	enemyBox.x = x;
	enemyBox.y = y;
	posX = x;

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

	default:
		break;
	}	
}

void Enemy::ApplyPhysics()
{
	cooldown--;
	switch (enemyType)
	{
	case GHOUL:
		if (flip)
			velX = -1.7f;
		if (!flip)
			velX = 1.7f;

		posX += velX;

		enemyBox.x = (int)round(posX);

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
		if (flip == false)
			tm->drawFrame("ghoul", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 32, 64, 1, frameCount, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("ghoul", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, 32, 64, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
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

void Enemy::takeHit()
{
	cooldown = 24;
	hitPoints--;
}

int Enemy::getCooldown()
{
	return cooldown;
}
