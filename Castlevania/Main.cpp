#include <stdlib.h>
#include <crtdbg.h>

#include <SDL.h>
#include "Game.h"

Game* game = 0;

int main(int argc, char* args[])
{	

	game = new Game();	

	while (game->isRunning())
	{
		game->HandleGameState();
		game->Render();		
	}


	delete game;
	
	return 0;
}

