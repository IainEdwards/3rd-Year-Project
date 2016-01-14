#include "Player.h"

Player::Player()
{
	PosX = PosY = 0;
	VelX = VelY = 0;
}

int Player::GetPosX()
{
	return PosX;
}

int Player::GetPosY()
{
	return PosY;
}

void Player::LoadContent(SDL_Renderer* renderer)
{
	playerTexture.LoadTexture("dot.bmp", renderer);
}

void Player::Draw(SDL_Renderer* renderer)
{
	playerTexture.DrawTexture(PosX, PosY, 0, 0, 0, renderer, SDL_FLIP_NONE);
}

void Player::Update(SDL_Event& e, Texture2D texture)
{
	GetInput(e);

	ApplyPhysics();

	//Draw(texture);
}

void Player::GetInput(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: VelY -= MaxMoveSpeed; break;
		case SDLK_DOWN: VelY += MaxMoveSpeed; break;
		case SDLK_LEFT: VelX -= MaxMoveSpeed; break;
		case SDLK_RIGHT: VelX += MaxMoveSpeed; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: VelY += MaxMoveSpeed; break;
		case SDLK_DOWN: VelY -= MaxMoveSpeed; break;
		case SDLK_LEFT: VelX += MaxMoveSpeed; break;
		case SDLK_RIGHT: VelX -= MaxMoveSpeed; break;
		}
	}
}

void Player::ApplyPhysics()
{
	PosX += VelX;

	//If the dot went too far to the left or right
	if ((PosX < 0) || (PosX + DOT_WIDTH > SCREEN_WIDTH))
	{
		//Move back
		PosX -= VelX;
	}

	//Move the dot up or down
	PosY += VelY;

	//If the dot went too far up or down
	if ((PosY < 0) || (PosY + DOT_HEIGHT > SCREEN_HEIGHT))
	{
		//Move back
		PosY -= VelY;
	}

}

