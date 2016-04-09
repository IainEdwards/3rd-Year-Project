#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include <SDL.h>
#include <SDL_mixer.h>

#include "Tile.h"
#include "SoundManager.h"
#include "Enemy.h"
#include "Boss.h"
#include "SubWeapon.h"
#include "LevelObject.h"
#include "DestroyableObject.h"

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
	FLASH,
	BLANK
};

class Player
{
public:

	Player();
	~Player() {};

	//void Load(TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm);

	void GetInput(SDL_Event& e);

	void ApplyPhysics(Tile *tiles[], std::list <LevelObject> levelObjects, std::list <DestroyableObject> objects);

	void PlaySounds(SoundManager* sm);

	void Draw(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount);

	void setCamera(SDL_Rect& camera);

	bool touchesWall(SDL_Rect box, Tile* tiles[]);	

	bool touchesStairs(SDL_Rect box, Tile* tiles[], bool goDown);

	bool touchesExit(SDL_Rect box, Tile* tiles[], bool first);

	bool touchesEntrance(SDL_Rect box, Tile* tiles[], bool first);

	bool touchesLevelObject(SDL_Rect box, std::list <LevelObject> levelObjects);

	bool touchesDestroyableObject(SDL_Rect box, std::list <DestroyableObject> objects);

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
	void SetFlip(bool setFlip);
	bool Crouching();
	bool Jumping();

	SDL_Rect AttackBox();
	SDL_Rect PlayerBox();	

	void SetLevelArea(int width, int height, int tiles);

	bool ReachedExit();
	bool ReachedDoor();
	bool ReachedExitStair();
	bool ReachedExitStair2();
	bool ReachedEntrance();
	bool ReachedEntrance2();	

	void SetOnStairs(bool up, bool right);

	//Health functions
	int Health();
	void SetHealth(int health);
	void takeHit(EnemyType enemyType, bool right);
	void takeHitBoss(BossType bossType, bool right);
	int HitCooldown();

	//Lives and death functions
	int Lives();
	void SetLives(int lives);
	void SetAlive();
	void playerDeath();
	bool PlayerDead();	
	bool Splash();

	//Ammo functions
	int Ammo();
	void SetAmmo(int ammo);

	SubWeaponType SubWeapon();
	void SetSubWeapon(SubWeaponType type);

	bool SpawnProjectile();
	void SetSpawnProjectile(bool value);
	int CurrentProjectiles();
	void SetCurrentProjectiles(int value);
	int MaxProjectiles();
	void SetMaxProjectiles(int value);	

	//Whip functions
	int WhipLevel();
	void SetWhipLevel(int level);	

	bool BossFight();

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

	bool stopSound;

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
	bool splash;

	//Variable for collecting whip
	float whipFlash;

	//Variable to turn on autowalk
	bool autoWalk;

	//Variable to turn on boss fight
	bool bossFight;
};
#endif