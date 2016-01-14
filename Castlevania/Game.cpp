#include "Game.h"

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
	int imgFlags = IMG_INIT_PNG;

	//Allow game loop to start running
	Running = true;

	Player.LoadContent(Renderer);

	//playerTexture.LoadTexture("dot.bmp", Renderer);	

	testTexture.LoadTexture("test.bmp", Renderer);
	
}

Game::~Game()
{
	SDL_DestroyWindow(Window);
	SDL_DestroyRenderer(Renderer);
	SDL_Quit();
}

void Game::HandleEvents()
{
	SDL_Event Event;

	while (Running)
		{
			//Handle events on queue
			while (SDL_PollEvent(&Event) != 0)
			{
				//User requests quit
				if (Event.type == SDL_QUIT)
				{
					Running = false;
				}
		
				//Handle input for the dot
				Player.GetInput(Event);
			}
		
			//Move the dot
			Player.ApplyPhysics();

			break;		
		}
}

void Game::Render()
{
	//Clear screen
	SDL_RenderClear(Renderer);		

	testTexture.DrawTexture(0, 0, 0, 0, 0, Renderer, SDL_FLIP_NONE);

	Player.Draw(Renderer);


	SDL_RenderPresent(Renderer);
}

void Game::Update()
{

}


