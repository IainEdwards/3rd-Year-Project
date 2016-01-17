#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <string>
#include "Tile.h"


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

	static const int MaxMoveSpeed = 10;
	static const int PLAYER_WIDTH = 32;
	static const int PLAYER_HEIGHT = 60;
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

	int PosX();
	int PosY();


private:

	SDL_Rect playerBox;

	int posX, posY;
	int velX, velY;

	float DoJump();
	void DoAttack();
	void HandleCollisions();

	

};
#endif