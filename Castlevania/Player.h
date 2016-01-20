#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <string>
#include "Tile.h"
#include "Timer.h"

enum PlayerAnimation
{
	IDLE,
	MOVING,
	JUMPING,
	ATTACKING
};

class Player
{
public:

	Player();
	~Player() {};

	void LoadContent(SDL_Renderer* renderer);

	void GetInput(SDL_Event& e);

	void ApplyPhysics(Tile *tiles[]);

	void setCamera(SDL_Rect& camera);

	bool touchesWall(SDL_Rect box, Tile* tiles[]);	

	bool checkCollision(SDL_Rect a, SDL_Rect b);
	

	void Reset(int x, int y);

	void OnKilled();


	//Player constants	
	
	static const int PLAYER_WIDTH = 32;
	static const int PLAYER_HEIGHT = 64;
	static const int PLAYER_OFFSET_X = 16;
	static const int PLAYER_OFFSET_Y = 4;	

	const int SCREEN_WIDTH = 512;
	const int SCREEN_HEIGHT = 448;

	const int LEVEL_WIDTH = 5632;
	const int LEVEL_HEIGHT = 448;

	//Tile constants
	const int TILE_WIDTH = 32;
	const int TILE_HEIGHT = 32;
	const int TOTAL_TILES = 2112;

	int CurrentFrame();

	PlayerAnimation Animation();

	int PosX();
	int PosY();

	bool Flip();

private:

	PlayerAnimation currentAnimation;	

	SDL_Rect playerBox;

	float posX, posY;
	float velX, velY;

	bool flip;
	
	void DoAttack();	

	//Player movement constants
	static const float MoveSpeed;
	static const float GravityAcceleration;	
	static const float JumpVelocity;
	static const int MaxJumpTime = 64;

	int currentFrame;
	int frameTime;
		
	bool jumping;	
	int jumpTime;
	bool jumpRelease;
	bool leftPress;
	bool rightPress;	

	bool attacking;
	int attackTime;
};
#endif