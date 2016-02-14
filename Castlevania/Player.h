#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include "Tile.h"
#include "SoundManager.h"
#include "Enemy.h"

enum PlayerAnimation
{
	IDLE,
	MOVING,
	JUMPING,
	ATTACKING,
	CROUCHING,
	CROUCH_ATTACK,
	STAIRS_UP,
	STAIRS_DOWN,
	STAIRS_UP_ATTACK,
	STAIRS_DOWN_ATTACK,
	DAMAGED
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

	bool touchesStairs(SDL_Rect box, Tile* tiles[], bool goDown);

	void moveToStairs(SDL_Rect box, Tile* tiles[]);

	void moveToStairsDown(SDL_Rect box, Tile* tiles[]);	

	void takeStep(bool up, bool right);

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

	int Health();
	void SetHealth(int health);
	void takeHit(EnemyType enemyType);

	int HitCooldown();

private:

	PlayerAnimation currentAnimation;	

	SDL_Rect playerBox;
	SDL_Rect attackBox;

	float posX, posY;
	float velY;// velX, velY;
	int velX;
	
	bool flip;	

	//Player movement constants
	static const int MoveSpeed;
	static const float GravityAcceleration;	
	static const float JumpVelocity;
	static const int MaxJumpTime = 64;

	//Variables for calculating animation frame
	int currentFrame;
	int frameTime;
		
	//Variables for calculating jumps
	bool jumping;	
	int jumpTime;
	bool jumpRelease;
	bool onGround;

	//Variables to prevent movement overlap
	bool leftPress;
	bool rightPress;	

	//Variables for attacking
	bool attacking;
	int attackTime;

	//Variable for crouching
	bool crouching;

	//Variables for stairs
	bool goToStairs;
	bool onStairs;
	bool upPress;
	bool downPress;
	bool rightStair;
	bool leftStair;	
	int stepFrame;
	int stepTime;
	int secondStep;
	bool stepUp;
	int cameraStep;
	int cameraStart;

	//Variables for level information
	int totalTiles;
	int levelWidth;
	int levelHeight;

	//Exit flag
	bool reachedExit;

	//Variables for player information
	int health;
	int lives;

	int hitCooldown;
	bool damaged;
};
#endif