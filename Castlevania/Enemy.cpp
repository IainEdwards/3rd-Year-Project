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

bool Enemy::Idle()
{
	return idle;
}

void Enemy::setEnemy(int x, int y, EnemyType type, bool setFlip)
{
	enemyBox.x = x;
	enemyBox.y = y;
	posX = (float)x;
	posY = (float)y;

	velX = velY = 0.0f;

	startX = x;
	startY = y;

	enemyType = type;

	flip = setFlip;

	splash = false;
	doJump = false;
	jumping = false;
	jumpTimer = 0;
	doShot = false;
	shooting = false;
	shootTimer = 0;
	shootCooldown = 0;
	cooldown = 0;
	deleteEnemy = false;

	switch (enemyType)
	{
	case GHOUL:				
		enemyBox.w = 32;
		enemyBox.h = 64;
		hitPoints = 1;
		break;

	case PANTHER:
		idle = true;
		enemyBox.w = 64;
		enemyBox.h = 32;
		hitPoints = 1;
		frameTimer = 0;
		frameCounter = 0;
		break;

	case BAT:
		idle = true;		
		enemyBox.w = 32;
		enemyBox.h = 32;
		hitPoints = 1;
		enemyTimer = 0;
		break;

	case BAT_BLUE:
		idle = true;
		enemyBox.w = 32;
		enemyBox.h = 32;
		hitPoints = 1;
		enemyTimer = 0;
		break;

	case FISHMAN:
		idle = false;
		doJump = true;
		enemyBox.w = 32;
		enemyBox.h = 64;
		hitPoints = 1;
		shootCooldown = 120;
		break;

	case SPEAR_KNIGHT:
		enemyBox.w = 32;
		enemyBox.h = 64;
		hitPoints = 3;
		frameTimer = 0;
		frameCounter = 0;
		enemyTimer = 0;
		break;

	case MEDUSA_HEAD:		
		enemyBox.w = 32;
		enemyBox.h = 32;
		hitPoints = 1;
		enemyTimer = 0;
		break;

	case BONE_TOWER:
		enemyBox.w = 32;
		enemyBox.h = 64;
		hitPoints = 11;
		enemyTimer = 0;
		frameCounter = 0;
		break;

	case SNAKE:
		enemyBox.w = 32;
		enemyBox.h = 20;
		hitPoints = 1;
		break;

	case PROJECTILE:		
		enemyBox.w = 16;
		enemyBox.h = 16;
		hitPoints = 1;
		break;

	case PROJECTILE_FAST:		
		enemyBox.w = 16;
		enemyBox.h = 16;
		hitPoints = 1;
		break;

	default:
		break;
	}	
}

void Enemy::ApplyPhysics(Tile *tiles[], int totalTiles, SDL_Rect playerBox)
{
	if (cooldown > 0)
		cooldown--;

	switch (enemyType)
	{
	case GHOUL:
		GhoulPhysics(tiles, totalTiles);
		break;

	case PANTHER:
		PantherPhysics(tiles, totalTiles, playerBox);
		break;

	case BAT:
		BatPhysics(playerBox);
		break;

	case BAT_BLUE:		
		BatPhysics(playerBox);
		break;

	case FISHMAN:
		FishmanPhysics(tiles, totalTiles);
		break;

	case SPEAR_KNIGHT:
		if (cooldown < 12)		
			SpearKnightPhysics(tiles, totalTiles, playerBox);
		break;

	case MEDUSA_HEAD:
		MedusaHeadPhysics();
		break;

	case BONE_TOWER:
		BoneTowerPhysics(playerBox);
		break;

	case SNAKE:
		if (!flip)
			enemyBox.x += 2;
		else
			enemyBox.x -= 2;

		if (touchesWall(tiles, totalTiles))
		{
			deleteEnemy = true;
		}

		break;

	case PROJECTILE:
		ProjectilePhysics(tiles, totalTiles, 1.0f);
		break;

	case PROJECTILE_FAST:
		ProjectilePhysics(tiles, totalTiles, 2.0f);
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
			tm->drawFrame("ghoul", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("ghoul", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case PANTHER:
		if (idle)
		{
			if (!flip)
				tm->draw("panther_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
			else
				tm->draw("panther_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (jumping)
		{
			if (!flip)
				tm->draw("panther_jumping", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
			else
				tm->draw("panther_jumping", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
		}
		else
		{
			if (!flip)
				tm->drawFrame("panther_running", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCounter, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("panther_running", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCounter, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	case BAT:
		if (idle)
		{
			if (!flip)
				tm->draw("bat_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
			else
				tm->draw("bat_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
		}
		else
		{
			if (!flip)
				tm->drawFrame("bat_flying", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("bat_flying", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	case BAT_BLUE:
		if (idle)
		{
			if (!flip)
				tm->draw("bat_idle_blue", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
			else
				tm->draw("bat_idle_blue", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
		}
		else
		{
			if (!flip)
				tm->drawFrame("bat_flying_blue", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("bat_flying_blue", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	case FISHMAN:
		if (jumping)
		{
			if (!flip)
				tm->draw("fishman_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
			else
				tm->draw("fishman_idle", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (shooting)
		{
			if (!flip)
				tm->draw("fishman_shooting", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
			else
				tm->draw("fishman_shooting", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
		}
		else
		{
			if (!flip)
				tm->drawFrame("fishman_moving", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("fishman_moving", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	case SPEAR_KNIGHT:
		if (!flip)
			tm->drawFrame("spear_knight", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCounter, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("spear_knight", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCounter, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case MEDUSA_HEAD:
		if (!flip)
			tm->drawFrame("medusa_head", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("medusa_head", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount % 2, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case BONE_TOWER:
		if (!flip)
			tm->drawFrame("bone_tower", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCounter % 2, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("bone_tower", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCounter % 2, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case SNAKE:
		if (!flip)
			tm->drawFrame("snake", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("snake", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, 1, frameCount % 2, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case PROJECTILE:
		if (!flip)
			tm->draw("projectile", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
		else
			tm->draw("projectile", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case PROJECTILE_FAST:
		if (!flip)
			tm->draw("projectile", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_NONE);
		else
			tm->draw("projectile", (int)floor(enemyBox.x) - camera.x, enemyBox.y - camera.y, enemyBox.w, enemyBox.h, renderer, SDL_FLIP_HORIZONTAL);
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

float Enemy::HitPoints()
{
	return hitPoints;
}

void Enemy::takeHit(int whipLevel)
{
	if (whipLevel == 1)
		hitPoints -= 1.4f;
	else
		hitPoints -= 2.0f;

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

bool Enemy::touchesWall(Tile* tiles[], int totalTiles)
{
	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == '1')
		{
			if (checkCollision(enemyBox, tiles[i]->Box()))
			{
				return true;
			}
		}

		if (tiles[i]->Type() == '2')
		{
			if ((enemyBox.y + 4) <= tiles[i]->Box().y)
			{
				if (checkCollision(enemyBox, tiles[i]->Box()))
				{
					return true;
				}
			}
		}

		if (tiles[i]->Type() == 'S' && enemyType != PANTHER)
		{
			if (checkCollision(enemyBox, tiles[i]->Box()))
			{
				return true;
			}
		}
		if (tiles[i]->Type() == 'E')
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

	posX += velX;
	enemyBox.x = (int)round(posX);
	if (touchesWall(tiles, totalTiles))
	{
		posX -= velX;

		flip = !flip;
	}
	enemyBox.x = (int)round(posX);


	enemyBox.y +=(int)round(velY);
	if (touchesWall(tiles, totalTiles))
	{
		enemyBox.y -= (int)round(velY);
		velY = 0.0f;		
	}	

	
}

void Enemy::PantherPhysics(Tile *tiles[], int totalTiles, SDL_Rect playerBox)
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

		if (enemyBox.x - playerBox.x < 128)
			idle = false;

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
			velY -= 5.0f;
			doJump = false;			
			jumping = true;			
		}

		if (jumping)
		{
			jumpTimer++;
			velX *= 1.4f;
		}
		posX += velX;
		enemyBox.x = (int)round(posX);

		if (touchesWall(tiles, totalTiles))
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

void Enemy::BatPhysics(SDL_Rect playerBox)
{
	if (idle && enemyBox.y - playerBox.y > -128 && enemyBox.x - playerBox.x > -160)
	{
		idle = false;
		enemyTimer = 0;
	}	

	if (idle)
	{
		velX = 0.0f;
		velY = 0.0f;
	}
	else
	{
		if (enemyType == BAT)
			posY = startY + 16 - (16 * sin(enemyTimer * 3 * 3.1415926f / 180.0f));

		if (enemyType == BAT_BLUE)
			posY = startY + (56 * sin(enemyTimer * 3 * 3.1415926f / 180.0f));

		enemyBox.y = (int)round(posY);

		if (enemyType == BAT)
		{
			if (flip)
				velX = -1.7f;
			if (!flip)
				velX = 1.7f;
		}

		if (enemyType == BAT_BLUE)
		{
			if (flip)
				velX = -3.4f;
			if (!flip)
				velX = 3.4f;
		}

		posX += velX;

		enemyBox.x = (int)round(posX);

		enemyTimer++;
	}
	
}

void Enemy::FishmanPhysics(Tile *tiles[], int totalTiles)
{
	shootCooldown--;

	velY += 0.5f;

	if (!flip)
		velX = 1.5f;
	else
		velX = -1.5f;

	enemyBox.y += (int)round(velY);

	if (doJump)
	{
		velY -= 16.0f;
		doJump = false;
		jumping = true;
	}

	if (jumping)
	{
		jumpTimer++;
		velX = 0.0f;
	}

	if (doShot && shootCooldown <= 0)
	{
		doShot = false;
		shooting = true;	
		shootCooldown = 300;
	}

	if (shooting)
	{
		shootTimer++;
		velX = 0.0f;
		if (shootTimer > 20)
		{
			shooting = false;
			shootTimer = 0;

			flip = !flip;
		}
	}	

	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == '6' && jumpTimer > 60 || tiles[i]->Type() == '6' && !jumping)
		{
			if (checkCollision(enemyBox, tiles[i]->Box()))
			{
				splash = true;
			}
		}
	}

	if (touchesWall(tiles, totalTiles) && velY >= 0.0f)
	{
		enemyBox.y -= (int)round(velY);
		velY = 0.0f;

		if (jumping)
		{
			jumpTimer = 0;
			jumping = false;
		}

		if (!jumping && shootCooldown <= 0)
		{
			doShot = true;			
		}
			
	}

	posX += velX;
	enemyBox.x = (int)round(posX);
	if (touchesWall(tiles, totalTiles))
	{
		posX -= velX;

		if (flip)
			flip = false;
		if (!flip)
			flip = true;
	}
	enemyBox.x = (int)round(posX);
		
}

void Enemy::SpearKnightPhysics(Tile *tiles[], int totalTiles, SDL_Rect playerBox)
{
	bool flipCheck = false;
	enemyTimer++;
	if (enemyTimer > 120)
	{
		if (playerBox.x % 4 == 0)
			flipCheck = false;
		else
			flipCheck = true;

		enemyTimer = 0;
	}
	
	
	int distY = enemyBox.y - playerBox.y;
	int distX = enemyBox.x - playerBox.x;
	if (distY > -10 && distY < 10 && distX > -160 && distX < 160 && enemyTimer % 90 == 0)
		if (enemyBox.x < playerBox.x)
			flip = false;
		else
			flip = true;
	else
		if (flipCheck)
			flip = !flip;

	frameTimer++;
	if (60 / frameTimer <= 4)
	{
		frameTimer = 0;
		frameCounter++;
		if (frameCounter >= 4)
			frameCounter = 0;
	}

	if (flip)
		velX = -1.0f;
	if (!flip)
		velX = 1.0f;

	posX += velX;
	enemyBox.x = (int)round(posX);
	if (touchesWall(tiles, totalTiles))
	{
		posX -= velX;
		flip = !flip;
	}
	enemyBox.x = (int)round(posX);	

}

void Enemy::MedusaHeadPhysics()
{

	posY = startY + (76 * sin(enemyTimer * 3 * 3.1415926f / 180.0f));
	enemyBox.y = (int)round(posY);
	
	if (flip)
		velX = -2.5f;
	if (!flip)
		velX = 2.5f;	

	posX += velX;
	enemyBox.x = (int)round(posX);

	enemyTimer++;
}

void Enemy::BoneTowerPhysics(SDL_Rect playerBox)
{

	if (playerBox.x < enemyBox.x)
		flip = false;
	else
		flip = true;

	int distX = enemyBox.x - playerBox.x;

	enemyTimer++;
	if (distX > -256 && distX < 256)
	{
		if ((enemyTimer > 240 && enemyTimer < 270) || (enemyTimer > 480 && enemyTimer < 510) || (enemyTimer > 600 && enemyTimer < 630))
			frameCounter++;
		else
			frameCounter = 0;
	}

	if (doShot)	
		doShot = false;


	if (distX > -256 && distX < 256)
	{
		if (enemyTimer == 270 || enemyTimer == 300 ||
			enemyTimer == 510 || enemyTimer == 540 ||
			enemyTimer == 630 || enemyTimer == 660)
		{
			doShot = true;
		}
	}

	if (enemyTimer > 720)
		enemyTimer = 0;
}

bool Enemy::Splash()
{
	return splash;
}

bool Enemy::DoShot()
{
	return doShot;
}

bool Enemy::DeleteEnemy()
{
	return deleteEnemy;
}

void Enemy::ProjectilePhysics(Tile *tiles[], int totalTiles, float speed)
{
	if (!flip)
		velX = 2.5f * speed;
	else
		velX = -2.5f * speed;

	posX += velX;
	enemyBox.x = (int)round(posX);
}