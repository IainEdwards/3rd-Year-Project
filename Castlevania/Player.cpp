#include "Player.h"

const float Player::MoveSpeed = 2.0f;
const float Player::GravityAcceleration = 0.3f;
const float Player::JumpVelocity = -6.5f;



Player::Player()
{
	playerBox.x = attackBox.x = 0;
	playerBox.y = attackBox.y = 0;
	playerBox.w = PLAYER_WIDTH;
	playerBox.h = PLAYER_HEIGHT;
	attackBox.w = 48;
	attackBox.h = 16;
	velX = velY = 0;	
	flip = false;
	jumping = false;
	jumpTime = 0;
	jumpRelease = false;	
	onGround = false;
	leftPress = false;
	rightPress = false;
	attacking = false;
	crouching = false;
	reachedExit = false;

	frameTime = 0;
	currentFrame = 0;

}

void Player::Load(TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm)
{
	tm->load("Assets/Sprites/Player/player_idle2.png", "player_idle", renderer);
	tm->load("Assets/Sprites/Player/player_moving.png", "player_moving", renderer);
	tm->load("Assets/Sprites/Player/player_jumping.png", "player_jumping", renderer);
	tm->load("Assets/Sprites/Player/player_whip1.png", "player_whip1", renderer);
	tm->load("Assets/Sprites/Player/player_crouching.png", "player_crouching", renderer);
	tm->load("Assets/Sprites/Player/player_crouch_whip1.png", "player_crouch_whip1", renderer);	

	sm->load("Assets/Sounds/whip1.wav", "whip1");	
	sm->setVolume("whip1", 32);
}

int Player::PosX()
{
	return playerBox.x;
}

int Player::PosY()
{
	return playerBox.y;
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

SDL_Rect Player::AttackBox()
{
	return attackBox;
}

SDL_Rect Player::PlayerBox()
{
	return playerBox;
}

void Player::GetInput(SDL_Event& e, SoundManager* sm)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
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
			crouching = true;
			break;

		case SDLK_x: 
			if (!jumping && !attacking && !crouching && onGround)
			{
				jumping = true;
				onGround = false;
				currentAnimation = JUMPING;
				jumpTime = 60;
				velY = JumpVelocity;
			}			
			break;
		case SDLK_z:
			if (!jumping && !attacking && !crouching)
			{				
				sm->play("whip1");
				currentFrame = 0;
				currentAnimation = ATTACKING;
				attacking = true;
				attackTime = 60;
				velX = 0;
			}
			else if (jumping && !attacking && !crouching)
			{				
				sm->play("whip1");
				currentFrame = 0;
				currentAnimation = ATTACKING;
				attacking = true;
				attackTime = 60;
			}
			else if (!jumping && !attacking && crouching)
			{				
				sm->play("whip1");
				currentFrame = 0;
				currentAnimation = CROUCH_ATTACK;
				attacking = true;
				attackTime = 60;
				velX = 0;
			}
			else if (jumping && !attacking && crouching)
			{				
				sm->play("whip1");
				currentFrame = 0;
				currentAnimation = ATTACKING;
				attacking = true;
				attackTime = 60;
			}
		}
	}
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
			crouching = false;
			break;
			
		}
	}
}

void Player::ApplyPhysics(Tile *tiles[])
{
	//Apply physics from key presses and set animation
	frameTime++;

	if (60 / frameTime <= 6)
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
		if (jumpTime <= 30 && !attacking)
		{
			currentAnimation = IDLE;
			velY += 0.7f;
		}
	}

	if (attacking)
	{
		attackTime--;
		if (attackTime <= 36)
		{
			attacking = false;
			currentAnimation = IDLE;
		}

		if (attackTime <= 48)
		{
			if (!flip)
			{
				if (!crouching || jumping)
				{
					attackBox.x = playerBox.x + PLAYER_WIDTH + 12;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
					attackBox.w = 44;
					attackBox.h = 16;
				}
				else
				{
					attackBox.x = playerBox.x + PLAYER_WIDTH + 12;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2);
					attackBox.w = 44;
					attackBox.h = 16;
				}				
			}
			else
			{
				if (!crouching || jumping)
				{
					attackBox.x = playerBox.x - 56;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2) - 16;
					attackBox.w = 44;
					attackBox.h = 16;
				}
				else
				{
					attackBox.x = playerBox.x - 56;
					attackBox.y = playerBox.y + (PLAYER_HEIGHT / 2);
					attackBox.w = 44;
					attackBox.h = 16;
				}				
			}
		}		
	}
	else
	{
		attackBox = { 0, 0, 0, 0 };
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
	velY += GravityAcceleration;

	playerBox.x += (int)round(velX);	

	//If the player reaches a wall
	if ((playerBox.x < 0) || (playerBox.x + PLAYER_WIDTH > levelWidth) || touchesWall(playerBox, tiles))
	{
		//Move back
		playerBox.x -= (int)round(velX);
	}

	//Move the player up or down
	playerBox.y += (int)round(velY);	

	//If the player touches floor or roof
	if ((playerBox.y < 0) || (playerBox.y + PLAYER_HEIGHT > levelHeight) || touchesWall(playerBox, tiles))
	{
		//Move back
		playerBox.y -= (int)round(velY);
		jumping = false;
		onGround = true;
		velY = 0;
		if (attacking)
			velX = 0;
	}
	else
		onGround = false;
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

		//If the tile is the exit
		if (tiles[i]->Type() == 'X')
		{
			if (checkCollision(box, tiles[i]->Box()))
			{
				reachedExit = true;				
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
	if (camera.x > levelWidth - camera.w)
	{
		camera.x = levelWidth - camera.w;
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

void Player::Draw(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	switch (Animation())
	{
	case IDLE:
		if (!flip)
			tm->draw("player_idle", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_NONE);
		else
			tm->draw("player_idle", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case MOVING:
		if (!flip)
			tm->drawFrame("player_moving", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, 1, frameCount, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_moving", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case JUMPING:
		if (!flip)
			tm->draw("player_jumping", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_NONE);
		else
			tm->draw("player_jumping", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case ATTACKING:
		if (!flip)
			tm->drawFrame("player_whip1", playerBox.x - camera.x - 32, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, 1, currentFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_whip1", playerBox.x - camera.x - 64, playerBox.y - camera.y + 2, 128, Player::PLAYER_HEIGHT, 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case CROUCHING:
		if (!flip)
			tm->draw("player_crouching", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_NONE);
		else
			tm->draw("player_crouching", playerBox.x - camera.x, playerBox.y - camera.y, Player::PLAYER_WIDTH, Player::PLAYER_HEIGHT, renderer, SDL_FLIP_HORIZONTAL);
		break;

	case CROUCH_ATTACK:
		if (!flip)
			tm->drawFrame("player_crouch_whip1", playerBox.x - camera.x - 32, playerBox.y - camera.y + 16, 128, Player::PLAYER_HEIGHT, 1, currentFrame, renderer, SDL_FLIP_NONE);
		else
			tm->drawFrame("player_crouch_whip1", playerBox.x - camera.x - 64, playerBox.y - camera.y + 16, 128, Player::PLAYER_HEIGHT, 1, currentFrame, renderer, SDL_FLIP_HORIZONTAL);
		break;

	default:
		break;
	}
}

bool Player::ReachedExit()
{
	return reachedExit;
}

void Player::SetLevelArea(int width, int height, int tiles)
{
	levelWidth = width;
	levelHeight = height;
	totalTiles = tiles;	
}