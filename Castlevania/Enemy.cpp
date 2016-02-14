#include "Enemy.h"

Enemy::Enemy()
{
	enemyBox.x = 0;
	enemyBox.y = 0;
	enemyBox.w = 32;
	enemyBox.h = 64;

	flip = true;
	hitPoints = 1;
	cooldown = 0;
}

Enemy::~Enemy()
{

}

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

	enemyType = type;

	switch (enemyType)
	{
	case GHOUL:
		hitPoints = 1;
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
			velX = -1.5f;
		if (!flip)
			velX = 1.5f;

		enemyBox.x += velX;

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
