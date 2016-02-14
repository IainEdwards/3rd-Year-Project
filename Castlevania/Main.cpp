#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <SDL.h>
#include "Game.h"

Game* game = 0;

int main(int argc, char* args[])
{
	//_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	game = new Game();	

	while (game->isRunning())
	{
		game->HandleGameState();
		game->Render();		
	}

	//_CrtDumpMemoryLeaks();

	delete game;
	
	return 0;
}

