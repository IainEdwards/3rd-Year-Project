#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include "Texture2D.h"
#include "Player.h"
#include <stdio.h>
#include <string>

class Game
{
public:

	Game();
	~Game();

	bool IsRunning() { return Running; }

	void HandleEvents();
	void Render();
	void Update();	
	

private:

	SDL_Window* Window;
	SDL_Renderer* Renderer;

	bool Running = false;

	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	Texture2D playerTexture;

	Player Player;	

	Texture2D testTexture;
};



#endif