#include "Game.h"
#include "Player.h"
#include "Level.h"
#include <iostream>
#include "TextureManager.h"
#include <fstream>

TextureManager *tm = new TextureManager();

Level level;

//Player player;

SDL_Window* Window;
SDL_Renderer* Renderer;

SDL_Rect camera;

float frameTime;
int frameCount;

Game::Game()
{
	//Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Create Window
	Window = SDL_CreateWindow("Castlevania", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	//Create vsynced Renderer for Window
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	//Set Renderer colour
	SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	//Initialise PNG loading
	//int imgFlags = IMG_INIT_PNG;

	//Allow game loop to start running
	running = true;

	//gameTime.Start();

	//Initialise the camera
	SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	level.LoadLevel("level1b", tm, Renderer);

	

	float frameTime = 0;
	int frameCount = 0;
	
}

Game::~Game()
{
	SDL_DestroyWindow(Window);
	SDL_DestroyRenderer(Renderer);	
}

void Game::HandleEvents()
{
	SDL_Event Event;	

	while (running)
		{
			//Handle events on queue
			while (SDL_PollEvent(&Event) != 0)
			{
				//User requests quit
				if (Event.type == SDL_QUIT)
				{
					running = false;
				}
		
				//Handle input for the player
				level.player.GetInput(Event);				
			}

			frameTime++;

			if (FPS / frameTime == 4)
			{
				frameTime = 0;
				frameCount += 1;
				if (frameCount >= 4)
					frameCount = 0;
			}

			level.UpdateLevel(camera);		

			break;		
		}
}

void Game::Render()
{
	//Clear screen
	SDL_RenderClear(Renderer);		

	//Draw level, includes player
	level.DrawLevel(tm, Renderer, camera);

	SDL_RenderPresent(Renderer);
}

bool Game::isRunning()
{
	return running;
}

void Game::Update()
{

}




