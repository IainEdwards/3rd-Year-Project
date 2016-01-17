#include <SDL.h>
#include "Game.h"

Game* game = 0;

int main(int argc, char* args[])
{
	game = new Game();

	while (game->isRunning())
	{
		game->HandleEvents();
		game->Render();
		game->Update();
	}

	delete game;
	
	return 0;
}

