#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include "Tile.h"
#include "SoundManager.h"

enum PlayerAnimation
{
	IDLE,
	MOVING,
	JUMPING,
	ATTACKING,
	CROUCHING,
	CROUCH_ATTACK
};

class Player
{
public:

	Player();
	~Player() {};

	void Load(TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm);

	void GetInput(SDL_Event& e, SoundManager* sm);

	void ApplyPhysics(Tile *tiles[]);

	void Draw(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	void setCamera(SDL_Rect& camera);

	bool touchesWall(SDL_Rect box, Tile* tiles[]);	

	bool checkCollision(SDL_Rect a, SDL_Rect b);	

	void Reset(int x, int y);



	//Player constants	
	
	static const int PLAYER_WIDTH = 32;
	static const int PLAYER_HEIGHT = 64;
	static const int PLAYER_OFFSET_X = 16;
	static const int PLAYER_OFFSET_Y = 4;	

	const int SCREEN_WIDTH = 512;
	const int SCREEN_HEIGHT = 448;

	//Tile constants
	const int TILE_WIDTH = 32;
	const int TILE_HEIGHT = 32;	

	int CurrentFrame();

	PlayerAnimation Animation();

	int PosX();
	int PosY();

	bool Flip();

	SDL_Rect AttackBox();
	SDL_Rect PlayerBox();	

	Mix_Chunk *whipSFX = NULL;

	void SetLevelArea(int width, int height, int tiles);

	bool ReachedExit();

private:

	PlayerAnimation currentAnimation;	

	SDL_Rect playerBox;
	SDL_Rect attackBox;

	float posX, posY;
	float velX, velY;

	bool flip;	

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
	bool onGround;

	bool leftPress;
	bool rightPress;	

	bool attacking;
	int attackTime;

	bool crouching;

	int totalTiles;
	int levelWidth;
	int levelHeight;

	bool reachedExit;
};
#endif