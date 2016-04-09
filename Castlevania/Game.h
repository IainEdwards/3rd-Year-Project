#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include "Player.h"


enum GameState
{
	Title,
	Intro,
	Fade,
	Running,
	Door,
	LevelComplete,
	Map,
	GameOver
};

class Game
{
public:

	Game();
	~Game();	

	void HandleGameState();
	void Update();
	void Render();	
	bool isRunning();

	void DrawHUD();

	void LoadAssets();	

	void RunTitle();
	void RunIntro();
	void RunDoor();
	void RunLevelComplete();
	void RunMap();
	void RunGameOver();

	void SetPlayerStats();

private:

	bool running = false;

	const int SCREEN_WIDTH = 512;
	const int SCREEN_HEIGHT = 448;	
	const int FPS = 60;
};



#endif