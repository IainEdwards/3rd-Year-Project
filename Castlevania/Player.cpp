#include "Player.h"

const int Player::MoveSpeed = 2;
const float Player::JumpVelocity = -6.5f;

Player::Player()
{
	playerBox.x = attackBox.x = 0;
	playerBox.y = attackBox.y = 0;
	playerBox.w = PLAYER_WIDTH;
	playerBox.h = PLAYER_HEIGHT;
	attackBox.w = 48;
	attackBox.h = 16;
}


int Player::PosX()
{
	return playerBox.x;
}
int Player::PosY()
{
	return playerBox.y;
}
void Player::ChangePosX(int value)
{
	playerBox.x += value;
}
bool Player::Flip()
{
	return flip;
}

int Player::Health()
{
	return health;
}
void Player::SetHealth(int value)
{
	health = value;
}

int Player::Lives()
{
	return lives;
}
void Player::SetLives(int value)
{
	lives = value;
}

int Player::Ammo()
{
	return ammo;
}
void Player::SetAmmo(int value)
{
	ammo = value;
}

SubWeaponType Player::SubWeapon()
{
	return currentSubWeapon;
}
void Player::SetSubWeapon(SubWeaponType type)
{
	currentSubWeapon = type;
}
bool Player::SpawnProjectile()
{
	return spawnProjectile;
}
void Player::SetSpawnProjectile(bool value)
{
	spawnProjectile = value;
}
int Player::CurrentProjectiles()
{
	return currentProjectiles;
}
void Player::SetCurrentProjectiles(int value)
{
	currentProjectiles = value;
}
void Player::SetMaxProjectiles(int value)
{
	maxProjectiles = value;
}

int Player::WhipLevel()
{
	return whipLevel;
}
void Player::SetWhipLevel(int value)
{
	whipLevel = value;
}

int Player::HitCooldown()
{
	return hitCooldown;
}

bool Player::PlayerDead()
{
	return playerDead;
}
void Player::SetAlive()
{
	playerDead = false;
}

PlayerAnimation Player::Animation()
{
	return currentAnimation;
}
int Player::CurrentFrame()
{
	return currentFrame;
}
void Player::SetCurrentFrame(int frame)
{
	currentFrame = frame;
}
void Player::SetCurrentAnimation(PlayerAnimation animation)
{
	currentAnimation = animation;
}

SDL_Rect Player::AttackBox()
{
	return attackBox;
}
SDL_Rect Player::PlayerBox()
{
	return playerBox;
}

void Player::GetInput(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && !autoWalk)
	{			
		switch (e.key.keysym.sym)
		{		
		case SDLK_LEFT: 
			leftPress = true;				
			break;			
			
		case SDLK_RIGHT:
			rightPress = true;			
			break;	

		case SDLK_DOWN:
			if (!onStairs)
				crouching = true;
			
			downPress = true;
			break;

		case SDLK_x: 
			if (!jumping && !attacking && !crouching && onGround && !onStairs)
			{
				jumping = true;
				onGround = false;
				currentAnimation = JUMPING;
				jumpTime = 60;
				velY = JumpVelocity;
			}			
			break;

		case SDLK_z:
			goToStairs = false;
			if (upPress && !jumping && !attacking && !throwing && !downPress && !onStairs && currentProjectiles < maxProjectiles && ammo > 0 && currentSubWeapon != EMPTY && currentSubWeapon != STOPWATCH)
			{
				currentFrame = 0;
				currentAnimation = THROWING;
				throwing = true;	
				attacking = true;
				attackTimer = 60;
				velX = 0;
				spawnProjectile = true;	
				ammo--;
			}
			else if (upPress && jumping && !attacking && !throwing && !downPress && !onStairs && currentProjectiles < maxProjectiles && ammo > 0 && currentSubWeapon != EMPTY && currentSubWeapon != STOPWATCH)
			{
				currentFrame = 0;
				currentAnimation = THROWING;
				throwing = true;
				attacking = true;
				attackTimer = 60;				
				spawnProjectile = true;
				ammo--;
			}
			else if (!jumping && !attacking && !throwing && !crouching && !onStairs && !damaged)
			{						
				currentFrame = 0;
				currentAnimation = ATTACKING;
				attacking = true;
				attackTimer = 60;
				velX = 0;
			}
			else if (jumping && !attacking && !throwing && !crouching && !onStairs && !damaged)
			{					
				currentFrame = 0;
				currentAnimation = ATTACKING;
				attacking = true;
				attackTimer = 60;
			}
			else if (!jumping && !attacking && !throwing && crouching && !onStairs && !damaged)
			{		
				currentFrame = 0;
				currentAnimation = CROUCH_ATTACK;
				attacking = true;
				attackTimer = 60;
				velX = 0;
			}
			else if (jumping && !attacking && !throwing && crouching && !onStairs && !damaged)
			{			
				currentFrame = 0;
				currentAnimation = ATTACKING;
				attacking = true;
				attackTimer = 60;
			}
			else if (onStairs && upPress && !downPress && !attacking && !throwing
				&& currentProjectiles < maxProjectiles && ammo > 0 && currentSubWeapon != EMPTY && currentSubWeapon != STOPWATCH)
			{
				currentFrame = 0;
				currentAnimation = STAIRS_UP_THROWING;
				throwing = true;
				attacking = true;
				attackTimer = 60;
				spawnProjectile = true;
				ammo--;
			}
			else if (onStairs && !attacking && !throwing && secondStep <= 0)
			{
				currentFrame = 0;				
				attacking = true;
				attackTimer = 60;

				if (rightStair && !flip)
				{
					currentAnimation = STAIRS_UP_ATTACK;
				}
				else if (rightStair && flip)
				{
					currentAnimation = STAIRS_DOWN_ATTACK;
				}
				else if (leftStair && flip)
				{
					currentAnimation = STAIRS_UP_ATTACK;
				}
				else if (leftStair && !flip)
				{
					currentAnimation = STAIRS_DOWN_ATTACK;
				}
			}
			
			break;

		case SDLK_UP:
			
			upPress = true;
			//currentAnimation = IDLE;

			if (!onStairs && !jumping && !attacking && !throwing)
				goToStairs = true;
			else
				goToStairs = false;
			
			

		}
	}
	//When a key is released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{		
		switch (e.key.keysym.sym)
		{		
		case SDLK_LEFT: 
			leftPress = false;
			break;
			
		case SDLK_RIGHT: 
			rightPress = false;
			break;

		case SDLK_DOWN:
			if (!onStairs)
				crouching = false;

			downPress = false;
			break;

		case SDLK_UP:
			
			goToStairs = false;		
			upPress = false;
			break;
		}
	}
}

//Apply physics from key presses and set animation
void Player::ApplyPhysics(Tile *tiles[])
{	
	//Set frames for animations
	frameTime++;
	if (60 / frameTime <= 6)
	{
		frameTime = 0;
		currentFrame += 1;
		if (currentFrame >= 4)
			currentFrame = 0;
	}

	//Reduce hit cooldown so player can take damage again
	hitCooldown--;
	if (hitCooldown <= 0)
		hitCooldown = 0;
	flashTimer--;
	if (flashTimer <= 0)
		flashTimer = 0;
	landingTimer--;
	if (landingTimer <= 0)
		landingTimer = 0;

	if (!attacking)
		attackBox = { 0, 0, 0, 0 };

	if (whipLevel == 1 || whipLevel == 2)
		whipLength = 44;
	else if (whipLevel == 3)
		whipLength = 76;

	//Set flip for animations
	if (!onStairs && hitCooldown <= 60)
	{
		if (velX > 0)
			flip = false;
		if (velX < 0)
			flip = true;
	}

	//Apply physics and set animations for when not on stairs
	if (!goToStairs && !onStairs)
	{
		if (jumping)
		{

			jumpTime--;
			if (jumpTime <= 30)
			{				
				velY += 0.7f;

				if (!attacking && !damaged)
					currentAnimation = IDLE;

				else if (!attacking && damaged)
					currentAnimation = DAMAGED;
			}			
		}

		if (attacking && !throwing)
		{
			attackTimer--;
			if (attackTimer <= 36)
			{
				attacking = false;
				currentAnimation = IDLE;
			}

			if (attackTimer <= 48)
			{
				if (!flip)
				{
					if (!crouching || jumping)
					{
						attackBox.x = playerBox.x + PLAYER_WIDTH + 12;
						attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
						attackBox.w = whipLength;
						attackBox.h = 16;
					}
					else
					{
						attackBox.x = playerBox.x + PLAYER_WIDTH + 12;
						attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2);
						attackBox.w = whipLength;
						attackBox.h = 16;
					}
				}
				else
				{
					if (!crouching || jumping)
					{
						attackBox.x = playerBox.x - (12 + whipLength);
						attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
						attackBox.w = whipLength;
						attackBox.h = 16;
					}
					else
					{
						attackBox.x = playerBox.x - (12 + whipLength);
						attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2);
						attackBox.w = whipLength;
						attackBox.h = 16;
					}
				}
			}
		}	
		else if (throwing && attacking)
		{
			attackTimer--;
			if (attackTimer <= 36)
			{				
				throwing = false;
				attacking = false;
				currentAnimation = IDLE;
			}
		}

		if (!jumping && !attacking && !crouching)
		{
			velY += 0.7f;

			if (rightPress && !leftPress)
			{
				velX = MoveSpeed;
				currentAnimation = MOVING;
			}

			if (!rightPress && leftPress)
			{
				velX = 0 - MoveSpeed;
				currentAnimation = MOVING;
			}

			if (!rightPress && !leftPress)
			{
				velX = 0;
				currentAnimation = IDLE;
			}

			if (rightPress && leftPress)
			{
				velX = 0;
				currentAnimation = IDLE;
			}
		}

		if (crouching && !attacking && !jumping)
		{
			velX = 0;
			currentAnimation = CROUCHING;
		}
		velY += 0.3f;

		if (autoWalk)
		{
			velX = 0;
			playerBox.x += 1;
			currentAnimation = MOVING;
		}

		playerBox.x += velX;

		//If the player reaches a wall
		if ((playerBox.x < 0) || (playerBox.x + PLAYER_WIDTH > levelWidth) || touchesWall(playerBox, tiles))
		{
			//Move back
			playerBox.x -= velX;
		}

		//Move the player up or down
		playerBox.y += (int)round(velY);

		//If the player touches floor or roof
		if ((playerBox.y < 0) || (playerBox.y + PLAYER_HEIGHT > levelHeight) || touchesWall(playerBox, tiles))
		{
			//Move back
			playerBox.y -= (int)round(velY);			

			if (velY > 15 && !damaged && !attacking && !jumping)
			{							
				velX = 0;
				landingTimer = 20;
			}

			jumping = false;
			onGround = true;

			if (landingTimer <= 18 && landingTimer > 0)
				crouching = true;

			velY = 0;
			if (attacking)
				velX = 0;

			if (damaged && !playerDead)
			{
				crouching = true;
				flashTimer = 80;
				hitCooldown = 80;
				damaged = false;
			}

			if (health <= 0)			
				playerDeath();			
				
			if (flashTimer <= 60 && landingTimer <= 0)
				if (!downPress)
					crouching = false;							
		}
		else
			onGround = false;
	}
	
	//Apply physics and set animatons for when on stairs
	if (onStairs)
		goToStairs = false;

	if (goToStairs)
	{		
		moveToStairs(playerBox, tiles);
	}	

	//Check to move into position to go down stairs
	if (crouching && touchesStairs(playerBox, tiles, true))
	{
		currentAnimation = MOVING;
		moveToStairsDown(playerBox, tiles);
	}

	//Movement for when on stairs, different for right and left stairs
	if (onStairs && !attacking)
	{
		secondStep--;

		if (rightStair)
		{
			if (secondStep == 0 && stepUp)
			{				
				takeStep(true, true);
				stepFrame = 0;				
			}

			if (secondStep == 0 && !stepUp)
			{				
				takeStep(false, false);
				stepFrame = 0;				
			}

			if (upPress && !downPress && !leftPress || rightPress && !leftPress && !downPress)
			{
				currentAnimation = STAIRS_UP;
				flip = false;				
				stepTime++;		

				if (stepTime % 16 == 0)
				{
					secondStep = 8;
					stepUp = true;
					takeStep(true, true);
					stepFrame = 1;	
				}					
			}		
			if (!upPress && downPress && !rightPress || leftPress && !rightPress && !upPress)
			{				
				currentAnimation = STAIRS_DOWN;
				flip = true;
				stepTime++;

				if (stepTime % 16 == 0)
				{
					secondStep = 8;
					stepUp = false;
					takeStep(false, false);
					stepFrame = 1;
				}			
			}						
		}
		if (leftStair)
		{
			if (secondStep == 0 && stepUp)
			{
				stepFrame++;
				takeStep(true, false);
				if (stepFrame >= 2)
					stepFrame = 0;
			}

			if (secondStep == 0 && !stepUp)
			{
				stepFrame++;
				takeStep(false, true);
				if (stepFrame >= 2)
					stepFrame = 0;
			}

			if (upPress && !downPress && !rightPress || leftPress && !rightPress && !downPress)
			{
				currentAnimation = STAIRS_UP;
				flip = true;
				stepTime++;		
				stepUp = true;

				if (stepTime % 16 == 0)
				{
					secondStep = 8;
					stepFrame++;
					takeStep(true, false);
					if (stepFrame >= 2)
						stepFrame = 0;
				}			
			}
			if (!upPress && downPress && !leftPress || rightPress && !leftPress && !upPress)
			{
				currentAnimation = STAIRS_DOWN;
				flip = false;
				stepTime++;
				stepUp = false;

				if (stepTime % 16 == 0)
				{		
					secondStep = 8;
					stepFrame++;
					takeStep(false, true);
					if (stepFrame >= 2)
						stepFrame = 0;
				}		
			}
		}		
	}

	if (onStairs && attacking && !throwing)
	{
		attackTimer--;

		if (rightStair)
		{			
			if (attackTimer <= 36)
			{
				attacking = false;
				
				if (!flip)
					currentAnimation = STAIRS_UP;
				else if (flip)
					currentAnimation = STAIRS_DOWN;
			}

			if (attackTimer <= 48)
			{
				if (!flip)
				{
					attackBox.x = playerBox.x + PLAYER_WIDTH + 12;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
					attackBox.w = 44;
					attackBox.h = 16;
				}
				else if (flip)
				{
					attackBox.x = playerBox.x - 56;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
					attackBox.w = 44;
					attackBox.h = 16;
				}				
			}
		}	
		if (leftStair)
		{
			if (attackTimer <= 36)
			{
				attacking = false;

				if (!flip)
					currentAnimation = STAIRS_DOWN;
				else if (flip)
					currentAnimation = STAIRS_UP;
			}

			if (attackTimer <= 48)
			{
				if (!flip)
				{
					attackBox.x = playerBox.x + PLAYER_WIDTH + 12;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
					attackBox.w = 44;
					attackBox.h = 16;
				}
				else if (flip)
				{
					attackBox.x = playerBox.x - 56;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
					attackBox.w = 44;
					attackBox.h = 16;
				}
			}
		}
	}
	else if (onStairs && throwing && attacking)
	{
		attackTimer--;
		if (attackTimer <= 36)
		{
			throwing = false;
			attacking = false;
			currentAnimation = STAIRS_UP;
		}
	}
	
	//Stop stair movement if player reaches top of stairs
	if (onStairs && !touchesStairs(playerBox, tiles, false))
	{
		if (rightStair)
		{
			onStairs = false;				
			playerBox.x -= 8;
		}
		if (leftStair)
		{
			onStairs = false;				
			playerBox.x += 8;
		}
		
	}

	//Stop stair movement when player reaches floor
	if (onStairs && touchesWall(playerBox, tiles))
	{
		if (rightStair)
		{
			if (downPress || leftPress)
			{
				onStairs = false;				
				playerBox.y -= 8;	
				playerBox.x += 8;				
			}
		}	
		if (leftStair)
		{
			if (downPress || rightPress)
			{
				onStairs = false;				
				playerBox.y -= 8;	
				playerBox.x -= 8;				
			}
		}
	}
	if (onStairs)
	{
		if (cameraStart + cameraStep < playerBox.x)
		{
			cameraStep++;
		}
		else if (cameraStart + cameraStep > playerBox.x)
		{
			cameraStep--;
		}

		if (touchesExit(playerBox, tiles, true))
		{
			reachedExitStair = true;
		}

		if (touchesExit(playerBox, tiles, false))
		{
			reachedExitStair2 = true;
		}

		if (touchesEntrance(playerBox, tiles, true))
		{
			reachedEntrance = true;
		}

		if (touchesEntrance(playerBox, tiles, false))
		{
			reachedEntrance2 = true;
		}
	}
}

void Player::PlaySounds(SoundManager* sm)
{
	if (attackTimer == 59 && !throwing)
		sm->play("player_whip");
	if (attackTimer == 59 && throwing)
		sm->play("player_throw");
	if (landingTimer == 20)
		sm->play("player_fall");
	if (reachedExit)
		sm->play("enter_castle");
}

bool Player::touchesWall(SDL_Rect box, Tile* tiles[])
{
	//Go through the tiles
	for (int i = 0; i < totalTiles; ++i)
	{
		//If the tile is a wall type tile
		if (tiles[i]->Type() == '1')
		{
			//If the collision box touches the wall tile
			if (checkCollision(box, tiles[i]->Box()))
			{
				return true;
			}
		}
		
		//If the tile is a platform type tile
		if (tiles[i]->Type() == '2')
		{
			if ((box.y + 4) <= tiles[i]->Box().y)
			{
				if (checkCollision(box, tiles[i]->Box()))
				{
					return true;
				}
			}
		}

		//Special case, if tile is next to top of stairs on a platform
		if (tiles[i]->Type() == 'S')
		{
			if ((box.y + 4) <= tiles[i]->Box().y)
			{
				SDL_Rect widerBox = tiles[i]->Box();
				widerBox.w = 48;
				widerBox.x -= 8;

				if (checkCollision(box, widerBox))
				{					
					if (!onStairs)
						return true;
					else
						return false;
				}
			}
		}

		//If the tile is the exit
		if (tiles[i]->Type() == 'X')
		{
			if (checkCollision(box, tiles[i]->Box()) && !onStairs)
			{
				reachedExit = true;				
			}
		}

		//If the tile is a door
		if (tiles[i]->Type() == 'D')
		{
			if (checkCollision(box, tiles[i]->Box()))
			{
				reachedDoor = true;
			}
		}

		//If the tile is near exit, first level
		if (tiles[i]->Type() == 'W')
		{
			if (checkCollision(box, tiles[i]->Box()))
			{				
				autoWalk = true;				
			}
		}

		if (tiles[i]->Type() == 'V')
		{
			if (checkCollision(box, tiles[i]->Box()))
			{				
				autoWalk = true;				
			}
		}
	}

	//If no wall tiles were touched
	return false;
}

bool Player::touchesStairs(SDL_Rect box, Tile* tiles[], bool goDown)
{
	if (!goDown)
	{
		for (int i = 0; i < totalTiles; ++i)
		{
			if (tiles[i]->Type() == 'R')
			{

				if (checkCollision(box, tiles[i]->Box()))
				{
					rightStair = true;
					leftStair = false;
					return true;
				}
			}
			if (tiles[i]->Type() == 'L')
			{
				if (checkCollision(box, tiles[i]->Box()))
				{
					rightStair = false;
					leftStair = true;
					return true;
				}
			}
		}
	}
	else if (goDown)
	{
		for (int i = 0; i < totalTiles; ++i)
		{
			if (tiles[i]->Type() == 'R')
			{
				Tile* tempTile = new Tile(tiles[i]->Box().x + 32, tiles[i]->Box().y - 16, 'R');
				if (tiles[i]->Box().y >= (box.y + box.h))
				{
					if (checkCollision(box, tempTile->Box()))
					{
						rightStair = true;
						leftStair = false;
						return true;
					}
				}
			}
			if (tiles[i]->Type() == 'L')
			{
				Tile* tempTile = new Tile(tiles[i]->Box().x - 32, tiles[i]->Box().y - 16, 'R');
				if (tiles[i]->Box().y >= (box.y + box.h))
				{
					if (checkCollision(box, tempTile->Box()))
					{
						leftStair = true;
						rightStair = false;
						return true;
					}
				}
			}			
		}
	}
	
	return false;
}

bool Player::touchesExit(SDL_Rect box, Tile* tiles[], bool first)
{
	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == 'X' && first)
		{
			if (checkCollision(box, tiles[i]->Box()))
			{
				return true;
			}
		}		
		if (tiles[i]->Type() == 'W' && !first)
		{
			if (checkCollision(box, tiles[i]->Box()))
			{
				return true;
			}
		}
	}

	return false;
}

bool Player::touchesEntrance(SDL_Rect box, Tile* tiles[], bool first)
{
	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == 'Z' && first)
		{
			if (checkCollision(box, tiles[i]->Box()))
			{
				return true;
			}
		}	
		if (tiles[i]->Type() == 'Y' && !first)
		{
			if (checkCollision(box, tiles[i]->Box()))
			{				
				return true;
			}
		}
	}

	return false;
}

void Player::moveToStairs(SDL_Rect box, Tile* tiles[])
{
	int playerRight = box.x + box.w;
	int playerLeft = box.x;
	int playerBase = box.y + box.h;

	//Go through the tiles
	for (int i = 0; i < totalTiles; ++i)
	{		
		if (tiles[i]->Type() == 'R')
		{
			if (playerBase == (tiles[i]->Box().y + tiles[i]->Box().h))
			{
				if (playerRight > tiles[i]->Box().x && playerRight < (tiles[i]->Box().x + tiles[i]->Box().w))
				{
					flip = false;
					currentAnimation = MOVING;
					playerBox.x += 1;
				}
				if (playerLeft > tiles[i]->Box().x && playerLeft < (tiles[i]->Box().x + tiles[i]->Box().w))
				{
					flip = true;
					currentAnimation = MOVING;
					playerBox.x -= 1;
				}
				if (playerLeft == tiles[i]->Box().x)
				{
					rightStair = true;
					leftStair = false;
					onStairs = true;					
					stepUp = true;		
					stepFrame = 0;	
					secondStep = 0;
					playerBox.x += 1;
					playerBox.y -= 1;
					cameraStart = playerBox.x;
					cameraStep = 0;
				}
			}			
		}
		if (tiles[i]->Type() == 'L')
		{
			if (playerBase == (tiles[i]->Box().y + tiles[i]->Box().h))
			{
				if (playerRight > tiles[i]->Box().x && playerRight < (tiles[i]->Box().x + tiles[i]->Box().w))
				{
					flip = false;
					currentAnimation = MOVING;
					playerBox.x += 1;
				}
				if (playerLeft > tiles[i]->Box().x && playerLeft < (tiles[i]->Box().x + tiles[i]->Box().w))
				{
					flip = true;
					currentAnimation = MOVING;
					playerBox.x -= 1;
				}
				if (playerLeft == tiles[i]->Box().x)
				{
					rightStair = false;
					leftStair = true;
					onStairs = true;
					stepUp = true;
					stepFrame = 0;
					secondStep = 0;
					playerBox.x -= 1;
					playerBox.y -= 1;
					cameraStart = playerBox.x;
					cameraStep = 0;
				}
			}
		}
		
	}	
}

void Player::moveToStairsDown(SDL_Rect box, Tile* tiles[])
{
	int playerRight = box.x + box.w;
	int playerLeft = box.x;	

	for (int i = 0; i < totalTiles; ++i)
	{
		if (tiles[i]->Type() == 'R')
		{
			Tile* tempTile = new Tile(tiles[i]->Box().x + 32, tiles[i]->Box().y - 16, 'R');

			if (playerRight > tempTile->Box().x && playerRight < (tempTile->Box().x + tempTile->Box().w))
			{
				//Not exactly sure why this works, I assume it needs to override a movement the other way
				playerBox.x += 2;
			}
			if (playerLeft > tempTile->Box().x && playerLeft < (tempTile->Box().x + tempTile->Box().w))
			{
				playerBox.x -= 1;
			}
			if (playerLeft == tempTile->Box().x)
			{
				onStairs = true;
				rightStair = true;
				leftStair = false;
				secondStep = 8;
				stepUp = false;
				takeStep(false, false);				
				crouching = false;
				stepFrame = 1;
				cameraStart = playerBox.x;
				cameraStep = 0;
			}
		}
		if (tiles[i]->Type() == 'L')
		{
			Tile* tempTile = new Tile(tiles[i]->Box().x - 32, tiles[i]->Box().y - 16, 'L');

			if (playerRight > tempTile->Box().x && playerRight < (tempTile->Box().x + tempTile->Box().w))
			{				
				playerBox.x += 1;
			}
			if (playerLeft > tempTile->Box().x && playerLeft < (tempTile->Box().x + tempTile->Box().w))
			{
				playerBox.x -= 2;
			}
			if (playerLeft == tempTile->Box().x)
			{
				onStairs = true;
				leftStair = true;
				rightStair = false;
				secondStep = 8;
				stepUp = false;
				takeStep(false, true);
				crouching = false;
				stepFrame = 1;
				cameraStart = playerBox.x;
				cameraStep = 0;
			}
		}
	}
}

//Player takes a step on the stairs
void Player::takeStep(bool up, bool right)
{	
	if (up && right)
	{
		playerBox.y -= 8;
		playerBox.x += 8;
	}
	if (!up && !right)
	{
		playerBox.y += 8;
		playerBox.x -= 8;
	}
	if (up && !right)
	{
		playerBox.y -= 8;
		playerBox.x -= 8;
	}
	if (!up && right)
	{
		playerBox.y += 8;
		playerBox.x += 8;
	}
}

bool Player::checkCollision(SDL_Rect a, SDL_Rect b)
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

void Player::setCamera(SDL_Rect& camera)
{
	//Center the camera over the player
	if (!onStairs)
		camera.x = (playerBox.x + PLAYER_WIDTH / 2) - SCREEN_WIDTH / 2;

	//If on stairs, smooth the camera movement
	if (onStairs)
		camera.x = (cameraStart + cameraStep + PLAYER_WIDTH / 2) - SCREEN_WIDTH / 2;
	
	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.x > levelWidth - camera.w)
	{
		camera.x = levelWidth - camera.w;
	}
}

void Player::Reset(int x, int y)//, int healthValue, int livesValue, int ammoValue)
{
	playerBox.x = x;
	playerBox.y = y;
	velX = 0;	
	velY = 0.0f;

	flip = false;
	jumping = false;
	jumpTime = 0;
	jumpRelease = false;
	landingTimer = 0;
	onGround = false;
	leftPress = false;
	rightPress = false;
	attacking = false;
	crouching = false;
	goToStairs = false;
	onStairs = false;
	upPress = false;
	downPress = false;
	rightStair = false;
	leftStair = false;
	stepFrame = 0;
	stepTime = 0;
	secondStep = 0;
	stepUp = false;
	cameraStep = 0;
	cameraStart = playerBox.x;	
	hitCooldown = 0;
	damaged = false;
	flashTimer = 0;
	whipFlash = 0.0f;
	reachedExit = false;
	reachedExitStair = false;
	reachedExitStair2 = false;
	reachedDoor = false;
	reachedEntrance = false;
	reachedEntrance2 = false;	
	playerDead = false;
	currentProjectiles = 0;
	maxProjectiles = 1;
	spawnProjectile = false;
	autoWalk = false;

	frameTime = 0;
	currentFrame = 0;
	currentAnimation = IDLE;
}

void Player::Draw(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	switch (Animation())
	{
	case IDLE:
		if (!flip)
			tm->drawFrame("player_idle", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, 0, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_idle", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, 0, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case MOVING:
		if (!flip)
			tm->drawFrame("player_moving", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, frameCount, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_moving", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case JUMPING:
		if (!flip)
			tm->drawFrame("player_jumping", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, 0, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_jumping", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, 0, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case ATTACKING:
		if (whipLevel == 1)
		{
			if (!flip)
				tm->drawFrame("player_whip1", playerBox.x - camera.x - 32, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_whip1", playerBox.x - camera.x - 64, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (whipLevel == 2)
		{
			if (!flip)
				tm->drawFrame("player_whip2", playerBox.x - camera.x - 32, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_whip2", playerBox.x - camera.x - 64, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (whipLevel == 3)
		{
			if (!flip)
				tm->drawFrame("player_whip3", playerBox.x - camera.x - 32, playerBox.y - camera.y + 2, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_whip3", playerBox.x - camera.x - 96, playerBox.y - camera.y + 2, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		
		break;

	case CROUCHING:
		if (!flip)
			tm->drawFrame("player_crouching", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, 0, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_crouching", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, 0, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case CROUCH_ATTACK:
		if (whipLevel == 1)
		{
			if (!flip)
				tm->drawFrame("player_crouch_whip1", playerBox.x - camera.x - 32, playerBox.y - camera.y + 16, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_crouch_whip1", playerBox.x - camera.x - 64, playerBox.y - camera.y + 16, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (whipLevel == 2)
		{
			if (!flip)
				tm->drawFrame("player_crouch_whip2", playerBox.x - camera.x - 32, playerBox.y - camera.y + 16, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_crouch_whip2", playerBox.x - camera.x - 64, playerBox.y - camera.y + 16, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		else if (whipLevel == 3)
		{
			if (!flip)
				tm->drawFrame("player_crouch_whip3", playerBox.x - camera.x - 32, playerBox.y - camera.y + 16, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_crouch_whip3", playerBox.x - camera.x - 96, playerBox.y - camera.y + 16, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	case STAIRS_UP:
		if (!flip)
			tm->drawFrame("player_stairs_up", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1 , stepFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_stairs_up", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, stepFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case STAIRS_DOWN:
		if (!flip)
			tm->drawFrame("player_stairs_down", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, stepFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_stairs_down", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, stepFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case STAIRS_UP_ATTACK:
		if (whipLevel == 1)
		{
			if (!flip)
				tm->drawFrame("player_stairs_up_whip1", playerBox.x - camera.x - 32 - 2, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_stairs_up_whip1", playerBox.x - camera.x - 64 + 2, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		if (whipLevel == 2)
		{
			if (!flip)
				tm->drawFrame("player_stairs_up_whip2", playerBox.x - camera.x - 32 - 2, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_stairs_up_whip2", playerBox.x - camera.x - 64 + 2, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		if (whipLevel == 3)
		{
			if (!flip)
				tm->drawFrame("player_stairs_up_whip3", playerBox.x - camera.x - 32 - 2, playerBox.y - camera.y, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_stairs_up_whip3", playerBox.x - camera.x - 96 + 2, playerBox.y - camera.y, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	case STAIRS_DOWN_ATTACK:
		if (whipLevel == 1)
		{
			if (!flip)
				tm->drawFrame("player_stairs_down_whip1", playerBox.x - camera.x - 32, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_stairs_down_whip1", playerBox.x - camera.x - 64, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		if (whipLevel == 2)
		{
			if (!flip)
				tm->drawFrame("player_stairs_down_whip2", playerBox.x - camera.x - 32, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_stairs_down_whip2", playerBox.x - camera.x - 64, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		if (whipLevel == 3)
		{
			if (!flip)
				tm->drawFrame("player_stairs_down_whip3", playerBox.x - camera.x - 32, playerBox.y - camera.y, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_NONE);
			else
				tm->drawFrame("player_stairs_down_whip3", playerBox.x - camera.x - 96, playerBox.y - camera.y, 160, Player::PLAYER_HEIGHT, (attackTimer % 4) + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		}
		break;

	case DAMAGED:
		if (!flip)
			tm->draw("player_damaged", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_NONE);
		else
			tm->draw("player_damaged", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case DEAD:
		if (!flip)
			tm->drawFrame("player_dead", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH + 32, Player::PLAYER_HEIGHT, 1, currentFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_dead", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH + 32, Player::PLAYER_HEIGHT, 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case FLASH:
		whipFlash += 0.5f;
		if (!flip)
			tm->drawFrame("player_flash", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, ((int)round(whipFlash) % 4) + 1, currentFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_flash", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, ((int)round(whipFlash) % 4) + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case THROWING:
		if (!flip)
			tm->drawFrame("player_throwing", playerBox.x - camera.x - 32, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_throwing", playerBox.x - camera.x - 64, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case STAIRS_UP_THROWING:
		if (!flip)
			tm->drawFrame("player_stairs_up_throwing", playerBox.x - camera.x - 32 - 2, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_stairs_up_throwing", playerBox.x - camera.x - 64 - 2, playerBox.y - camera.y, 128, Player::PLAYER_HEIGHT, flashTimer % 2 + 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;


	default:
		break;
	}
}

bool Player::ReachedExit()
{
	return reachedExit;
}

bool Player::ReachedDoor()
{
	return reachedDoor;
}

bool Player::ReachedExitStair()
{
	return reachedExitStair;
}

bool Player::ReachedExitStair2()
{
	return reachedExitStair2;
}

bool Player::ReachedEntrance()
{
	return reachedEntrance;
}

bool Player::ReachedEntrance2()
{
	return reachedEntrance2;
}

void Player::SetOnStairs(bool up)
{
	if (up)
	{
		onStairs = true;
		leftStair = true;
		rightStair = false;
		currentAnimation = STAIRS_UP;
		flip = true;
	}

	if (!up)
	{
		onStairs = true;
		leftStair = true;
		rightStair = false;
		currentAnimation = STAIRS_DOWN;
	}
}

void Player::SetLevelArea(int width, int height, int tiles)
{
	levelWidth = width;
	levelHeight = height;
	totalTiles = tiles;	
}

void Player::takeHit(EnemyType enemyType)
{
	switch (enemyType)
	{
	case GHOUL:
		health -= 2;
		break;

	default:
		break;
	}

	if (!onStairs)
	{
		currentAnimation = DAMAGED;
		hitCooldown = 120;
		damaged = true;
		jumping = true;
		onGround = false;
		jumpTime = 60;
		velY = -4.5f;

		if (!flip)
			velX = -2;
		else
			velX = 2;
	}	
	else
	{
		hitCooldown = 80;
		flashTimer = 80;
	}
	
}

void Player::playerDeath()
{
	currentFrame = 0;
	playerDead = true;
	velX = 0;
	velY = 0.0f;
	currentAnimation = DEAD;
	currentSubWeapon = EMPTY;
	lives--;
}