#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <string>
#include "Texture2D.h"


class Player
{
public:

	Player();
	~Player() {};

	void Update(SDL_Event& e, Texture2D texture);
	void Draw(SDL_Renderer* renderer);
	void GetInput(SDL_Event& e);
	void LoadContent(SDL_Renderer* renderer);
	
	void ApplyPhysics();

	void OnKilled();

	static const int MaxMoveSpeed = 10;
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	int GetPosX();
	int GetPosY();


private:

	int PosX, PosY;
	int VelX, VelY;

	float DoJump();
	void DoAttack();
	void HandleCollisions();

	Texture2D playerTexture;

	

};
#endif