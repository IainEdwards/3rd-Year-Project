#include "Player.h"

const float Player::MoveSpeed = 2.0f;
const float Player::GravityAcceleration = 0.3f;
const float Player::JumpVelocity = -6.5f;


// Gain access to keystate array
//const Uint8 *keys = SDL_GetKeyboardState(NULL);

Player::Player()
{
	playerBox.x = 0;// PLAYER_OFFSET_X;
	playerBox.y = 0;// PLAYER_OFFSET_Y;
	playerBox.w = PLAYER_WIDTH;
	playerBox.h = PLAYER_HEIGHT;
	velX = velY = 0;
	//posX = posY = 0;
	flip = false;
	jumping = false;
	jumpTime = 0;
	jumpRelease = false;	
	leftPress = false;
	rightPress = false;
	attacking = false;

	frameTime = 0;
	currentFrame = 0;
}

int Player::PosX()
{
	return playerBox.x;// posX;// -PLAYER_OFFSET_X;// playerBox.x - PLAYER_OFFSET_X;
}

int Player::PosY()
{
	return playerBox.y;// posY;// -PLAYER_OFFSET_Y;// playerBox.y - PLAYER_OFFSET_Y;
}

bool Player::Flip()
{
	return flip;
}

PlayerAnimation Player::Animation()
{
	return currentAnimation;
}

int Player::CurrentFrame()
{
	return currentFrame;
}

void Player::GetInput(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{			
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{		
		case SDLK_LEFT: 
			leftPress = true;				
			break;			
			
		case SDLK_RIGHT:
			rightPress = true;			
			break;			
			
		case SDLK_x: 
			if (!jumping && !attacking)
			{
				jumping = true;
				currentAnimation = JUMPING;
				jumpTime = 60;
				velY = JumpVelocity;
			}			
			break;
		case SDLK_z:
			if (!jumping && !attacking)
			{
				currentFrame = 0;
				currentAnimation = ATTACKING;
				attacking = true;
				attackTime = 60;
				velX = 0;
			}
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{		
		case SDLK_LEFT: 
			leftPress = false;
			break;
			
		case SDLK_RIGHT: 
			rightPress = false;
			break;
			
		}
	}
}

void Player::ApplyPhysics(Tile *tiles[])
{
	//Apply physics from key presses and set animation

	frameTime++;

	if (60 / frameTime <= 4)
	{
		frameTime = 0;
		currentFrame += 1;
		if (currentFrame >= 4)
			currentFrame = 0;
	}

	if (velX > 0)
		flip = false;
	if (velX < 0)
		flip = true;

	if (jumping)
	{
		jumpTime--;
		if (jumpTime <= 30)
		{
			currentAnimation = IDLE;
			velY += 0.7f;
		}
	}

	if (attacking)
	{
		attackTime--;
		if (attackTime <= 20)
		{
			attacking = false;
			currentAnimation = IDLE;
		}
	}

	if (!jumping && !attacking)
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
	velY += GravityAcceleration;

	playerBox.x += (int)round(velX);	

	//If the player went too far to the left or right
	if ((playerBox.x < 0) || (playerBox.x + PLAYER_WIDTH > LEVEL_WIDTH) || touchesWall(playerBox, tiles))
	{
		//Move back
		playerBox.x -= (int)round(velX);
	}

	//Move the player up or down
	playerBox.y += (int)round(velY);	

	//If the player went too far up or down
	if ((playerBox.y < 0) || (playerBox.y + PLAYER_HEIGHT > LEVEL_HEIGHT) || touchesWall(playerBox, tiles))
	{
		//Move back
		playerBox.y -= (int)round(velY);
		jumping = false;
		velY = 0;			
	}	
}

void Player::DoAttack()
{
	attacking = true;
	attackTime = 30;
}


bool Player::touchesWall(SDL_Rect box, Tile* tiles[])
{
	//Go through the tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		//If the tile is a wall type tile
		if (tiles[i]->Type() == 1)
		{
			//If the collision box touches the wall tile
			if (checkCollision(box, tiles[i]->Box()))
			{
				return true;
			}
		}
		
		//If the tile is a platform type tile
		if (tiles[i]->Type() == 2)
		{
			if ((box.y + 4) <= tiles[i]->Box().y)
			{
				if (checkCollision(box, tiles[i]->Box()))
				{
					return true;
				}
			}
		}
	}

	//If no wall tiles were touched
	return false;
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
	camera.x = (playerBox.x + PLAYER_WIDTH / 2) - SCREEN_WIDTH / 2;	

	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}

	//Removed options for camera motion on y axis
	//camera.y = (playerBox.y + PLAYER_HEIGHT / 2) - SCREEN_HEIGHT / 2;
	/*if (camera.y < 0)
	{
		camera.y = 0;
	}
	
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}*/
}

void Player::Reset(int x, int y)
{
	playerBox.x = x;
	playerBox.y = y;
	velX = velY = 0;
	//posX = x;
	//posY = y;
}