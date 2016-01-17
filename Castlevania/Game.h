#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include "Player.h"
#include "Timer.h"
#include <stdio.h>
#include <string>

class Game
{
public:

	Game();
	~Game();	

	void HandleEvents();
	void Render();
	void Update();	
	bool isRunning();

private:

	bool running = false;

	const int SCREEN_WIDTH = 512;
	const int SCREEN_HEIGHT = 448;	
	const int FPS = 60;
};



#endif