#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include "Tile.h"
#include "SoundManager.h"
#include "Enemy.h"
#include "SubWeapon.h"

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
	THROWING,
	STAIRS_UP_THROWING,
	DAMAGED,
	DEAD,
	FLASH
};

class Player
{
public:

	Player();
	~Player() {};

	//void Load(TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm);

	void GetInput(SDL_Event& e);

	void ApplyPhysics(Tile *tiles[]);

	void PlaySounds(SoundManager* sm);

	void Draw(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	void setCamera(SDL_Rect& camera);

	bool touchesWall(SDL_Rect box, Tile* tiles[]);	

	bool touchesStairs(SDL_Rect box, Tile* tiles[], bool goDown);

	bool touchesExit(SDL_Rect box, Tile* tiles[], bool first);

	bool touchesEntrance(SDL_Rect box, Tile* tiles[], bool first);

	void moveToStairs(SDL_Rect box, Tile* tiles[]);

	void moveToStairsDown(SDL_Rect box, Tile* tiles[]);	

	void takeStep(bool up, bool right);

	bool checkCollision(SDL_Rect a, SDL_Rect b);	

	void Reset(int x, int y);// , int health, int lives, int ammo);

	//Player constants		
	static const int PLAYER_WIDTH = 32;
	static const int PLAYER_HEIGHT = 64;	

	//Screen constants
	const int SCREEN_WIDTH = 512;
	const int SCREEN_HEIGHT = 448;

	//Tile constants
	const int TILE_WIDTH = 32;
	const int TILE_HEIGHT = 32;	

	int CurrentFrame();
	void SetCurrentFrame(int frame);
	void SetCurrentAnimation(PlayerAnimation animation);
	PlayerAnimation Animation();

	int PosX();
	int PosY();
	void ChangePosX(int value);

	bool Flip();

	SDL_Rect AttackBox();
	SDL_Rect PlayerBox();	

	void SetLevelArea(int width, int height, int tiles);

	bool ReachedExit();
	bool ReachedDoor();
	bool ReachedExitStair();
	bool ReachedExitStair2();
	bool ReachedEntrance();
	bool ReachedEntrance2();	

	void SetOnStairs(bool up);

	//Health functions
	int Health();
	void SetHealth(int health);
	void takeHit(EnemyType enemyType);
	int HitCooldown();

	//Lives and death functions
	int Lives();
	void SetLives(int lives);
	void SetAlive();
	void playerDeath();
	bool PlayerDead();	

	//Ammo functions
	int Ammo();
	void SetAmmo(int ammo);

	SubWeaponType SubWeapon();
	void SetSubWeapon(SubWeaponType type);

	bool SpawnProjectile();
	void SetSpawnProjectile(bool value);
	int CurrentProjectiles();
	void SetCurrentProjectiles(int value);
	void SetMaxProjectiles(int value);	

	//Whip functions
	int WhipLevel();
	void SetWhipLevel(int level);	

private:

	PlayerAnimation currentAnimation;	

	SubWeaponType currentSubWeapon;
	int maxProjectiles;
	int currentProjectiles;
	bool spawnProjectile;

	SDL_Rect playerBox;
	SDL_Rect attackBox;

	float posX, posY;
	float velY;
	int velX;
	
	bool flip;	

	//Player movement constants
	static const int MoveSpeed;	
	static const float JumpVelocity;	

	//Variables for calculating animation frame
	int currentFrame;
	int frameTime;
		
	//Variables for calculating jumps
	bool jumping;	
	int jumpTime;
	bool jumpRelease;
	bool onGround;
	int landingTimer;

	//Variables to prevent movement overlap
	bool leftPress;
	bool rightPress;	

	//Variables for attacking
	bool attacking;
	int attackTimer;

	bool throwing;

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
	bool reachedDoor;
	bool reachedExitStair;
	bool reachedExitStair2;
	bool reachedEntrance;
	bool reachedEntrance2;
	

	//Variables for player information
	int health;
	int lives;
	int ammo;
	int whipLevel;
	int whipLength;

	//Variables for player taking damage
	int hitCooldown;
	bool damaged;
	int flashTimer;
	bool playerDead;	

	//Variable for collecting whip
	float whipFlash;

	//Variable to turn on autowalk
	bool autoWalk;
};
#endif