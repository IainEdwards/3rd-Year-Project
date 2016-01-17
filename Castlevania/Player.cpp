#include "Player.h"

Player::Player()
{
	playerBox.x = PLAYER_OFFSET_X;
	playerBox.y = PLAYER_OFFSET_Y;
	playerBox.w = PLAYER_WIDTH;
	playerBox.h = PLAYER_HEIGHT;
	velX = velY = 0;
}

int Player::PosX()
{
	return playerBox.x - PLAYER_OFFSET_X;
}

int Player::PosY()
{
	return playerBox.y - PLAYER_OFFSET_Y;
}

void Player::GetInput(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: velY -= MaxMoveSpeed; break;
		case SDLK_DOWN: velY += MaxMoveSpeed; break;
		case SDLK_LEFT: velX -= MaxMoveSpeed; break;
		case SDLK_RIGHT: velX += MaxMoveSpeed; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: velY += MaxMoveSpeed; break;
		case SDLK_DOWN: velY -= MaxMoveSpeed; break;
		case SDLK_LEFT: velX += MaxMoveSpeed; break;
		case SDLK_RIGHT: velX -= MaxMoveSpeed; break;
		}
	}
}

void Player::ApplyPhysics(Tile *tiles[])
{
	playerBox.x += velX;

	//If the player went too far to the left or right
	if ((playerBox.x < 0) || (playerBox.x + PLAYER_WIDTH > LEVEL_WIDTH) || touchesWall(playerBox, tiles))
	{
		//Move back
		playerBox.x -= velX;
	}

	//Move the dot up or down
	playerBox.y += velY;

	//If the dot went too far up or down
	if ((playerBox.y < 0) || (playerBox.y + PLAYER_HEIGHT > LEVEL_HEIGHT) || touchesWall(playerBox, tiles))
	{
		//Move back
		playerBox.y -= velY;
	}

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
}