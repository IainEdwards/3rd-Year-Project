#include <iostream>
#include <fstream>
#include <vector>
#include "SDL_mixer.h"

#include "Game.h"
#include "Player.h"
#include "Level.h"
#include "TextureManager.h"
#include "SoundManager.h"

GameState gameState = GameState::Fade;

TextureManager *tm = new TextureManager();
SoundManager *sm = new SoundManager();

Level levels[3];
int levelN = 1;

int health, lives, ammo, stage, bosshealth;
int totalScore, levelScore, prevScore;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Rect camera;

Mix_Music* music = NULL;

//Variables for title
float frameTime;
int frameCount, frameRow;
int titleTime, titleCount;
int introTime;
bool enterPressed = false;

//Variables for intro
int posX, posY = 0;
float cloudX = 0;
int cloudY = 0;
float bat1X, bat1Y = 0;
float bat2X, bat2Y = 0;

int gameTime;

Uint32 currentTime, deltaTime, lastUpdate;



Game::Game()
{
	//Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Create Window
	window = SDL_CreateWindow("Castlevania", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	//Create vsynced Renderer for Window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	//Set Renderer colour
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	//Allow game loop to start running
	running = true;

	LoadAssets();	

	//Initialise the camera
	camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

}

Game::~Game()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);	
}

void Game::HandleGameState()
{
	switch (gameState)
	{
	case GameState::Title:
		RunTitle();
		break;
	
	case GameState::Intro:
		RunIntro();		
		break;

	case GameState::Fade:

		fadeTime--;
		if (fadeTime <= 0)
			fadeDone = true;

		if (fadeDone)
		{			
			levels[levelN];
			levelN++;

			std::string levelID = "level";
			levelID.append(std::to_string(levelN));

			music = Mix_LoadMUS("Assets/Music/level1.ogg");
			
			levels[levelN].LoadLevel(levelID, levelN, tm, renderer, sm);	
			gameTime = 300;		
			health = 16;
			levelScore = 0;
			prevScore = totalScore;
			ammo = 5;
			lives = 3;
			stage = 1;
			bosshealth = 16;
			gameState = GameState::Running;
		}
		break;
			
	
	case GameState::Running:		
		Update();		

		if (levels[levelN].ReachedExit())
		{					
			fadeTime = 60;
			fadeDone = false;
			gameState = GameState::Fade;			
		}

		break;
	
	default:
		break;
	}		
	
}

void Game::Update()
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
			levels[levelN].player.GetInput(Event, sm);			
		}

		frameTime++;
		if (FPS / frameTime == 6)
		{
			frameTime = 0;
			frameCount++;
			if (frameCount >= 4)
				frameCount = 0;
		}

		levels[levelN].UpdateLevel(camera);	

		if (Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(music, 0);
			Mix_VolumeMusic(64);
		}

		//Calculate change in time for accurate game timer
		currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastUpdate;

		if (deltaTime >= 1000)
		{
			lastUpdate = currentTime;
			gameTime--;
		}

		break;		
	}
}


void Game::Render()
{
	//Clear screen
	SDL_RenderClear(renderer);

	switch (gameState)
	{
	case GameState::Title:

		//Draw Title screen and animated bat, text flashes on enter
		tm->draw("title", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);

		tm->drawFrame("titlebat2", 368, 192, 144, 112, frameRow, frameCount, renderer, SDL_FLIP_NONE);	

		if (enterPressed)
			tm->drawFrame("titlecover", 145, 255, 224, 16, 1, titleCount, renderer, SDL_FLIP_NONE);		

		break;

	case GameState::Intro:

		//Draw intro screen, player walks across and stops in center, 2 bats and a cloud animated in background
		tm->draw("intro", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);

		if (posX > 240)
			tm->drawFrame("player_introwalk", posX, posY, 32, 64, 1, frameCount / 2, renderer, SDL_FLIP_HORIZONTAL);
		else
			tm->draw("player_intro", posX, posY + 2, 32, 64, renderer, SDL_FLIP_HORIZONTAL);

		tm->draw("introcloud", (int) floor(cloudX), cloudY, 64, 32, renderer, SDL_FLIP_NONE);
		tm->draw("introcover", 480, 128, 32, 64, renderer, SDL_FLIP_NONE);
		tm->drawFrame("introbat", (int)floor(bat1X), (int)floor(bat1Y), 16, 16, 1, frameCount, renderer, SDL_FLIP_NONE);
		tm->drawFrame("introbat", (int)floor(bat2X), (int)floor(bat2Y), 16, 16, 1, frameCount, renderer, SDL_FLIP_NONE);

		break;

	case GameState::Fade:

		//Draw blank screen to fade in and out of levels
		tm->draw("fade", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);
		break;

	case GameState::Running:

		//Draw level, includes player, enemies etc
		levels[levelN].DrawLevel(tm, renderer, camera, frameCount);		

		levelScore = levels[levelN].Score();
		totalScore = levelScore + prevScore;

		//Draw the HUD
		DrawHUD();
		break;
	
	default:
		break;
	}		

	SDL_RenderPresent(renderer);
}

void Game::DrawHUD()
{
	tm->draw("hudbase", 0, 0, SCREEN_WIDTH, 80, renderer, SDL_FLIP_NONE);

	//Draw Time, takes each digit from game time and takes correct frame from image to represent that number
	tm->drawFrame("numbers", 289, 15, 16, 16, 1, gameTime / 1000 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 305, 15, 16, 16, 1, gameTime / 100 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 321, 15, 16, 16, 1, gameTime / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 337, 15, 16, 16, 1, gameTime % 10, renderer, SDL_FLIP_NONE);

	//Draw Score
	tm->drawFrame("numbers", 97, 15, 16, 16, 1, totalScore / 100000 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 113, 15, 16, 16, 1, totalScore / 10000 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 129, 15, 16, 16, 1, totalScore / 1000 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 145, 15, 16, 16, 1, totalScore / 100 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 161, 15, 16, 16, 1, totalScore / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 177, 15, 16, 16, 1, totalScore % 10, renderer, SDL_FLIP_NONE);

	//Draw ammo
	tm->drawFrame("numbers", 369, 31, 16, 16, 1, ammo / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 385, 31, 16, 16, 1, ammo % 10, renderer, SDL_FLIP_NONE);

	//Draw lives
	tm->drawFrame("numbers", 369, 47, 16, 16, 1, lives / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 385, 47, 16, 16, 1, lives % 10, renderer, SDL_FLIP_NONE);

	//Draw stage
	tm->drawFrame("numbers", 465, 15, 16, 16, 1, stage / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 481, 15, 16, 16, 1, stage % 10, renderer, SDL_FLIP_NONE);

	//Draw health
	for (int i = 0; i < health; i++)
		tm->draw("health", 112 + i * 8, 34, 8, 12, renderer, SDL_FLIP_NONE);

	//Draw bosshealth
	for (int i = 0; i < health; i++)
		tm->draw("bosshealth", 112 + i * 8, 50, 8, 12, renderer, SDL_FLIP_NONE);
}

void Game::LoadAssets()
{
	float frameTime = 0;
	int frameCount = 0;

	//Assets for title screen
	tm->load("Assets/Backgrounds/title.png", "title", renderer);
	tm->load("Assets/Sprites/Misc/titlebat2.png", "titlebat2", renderer);	
	tm->load("Assets/Sprites/Misc/titlecover.png", "titlecover", renderer);
	frameRow = 1;

	//Assets for intro screen
	music = Mix_LoadMUS("Assets/Music/level0.ogg");
	tm->load("Assets/Backgrounds/intro.png", "intro", renderer);
	tm->load("Assets/Sprites/Player/player_intro.png", "player_intro", renderer);
	tm->load("Assets/Sprites/Player/player_moving.png", "player_introwalk", renderer);
	tm->load("Assets/Sprites/Misc/introcover.png", "introcover", renderer);
	tm->load("Assets/Sprites/Misc/introcloud.png", "introcloud", renderer);
	tm->load("Assets/Sprites/Misc/introbat.png", "introbat", renderer);

	//Assets for fade screen
	tm->load("Assets/Sprites/Misc/fade.png", "fade", renderer);
	fadeDone = false;
	fadeTime = 60;

	//Assets for HUD
	tm->load("Assets/HUD/hudbase.png", "hudbase", renderer);
	tm->load("Assets/HUD/numbers.png", "numbers", renderer);
	tm->load("Assets/HUD/health.png", "health", renderer);
	tm->load("Assets/HUD/bosshealth.png", "bosshealth", renderer);

	totalScore = 0;
	prevScore = 0;
}

bool Game::isRunning()
{
	return running;
}

void Game::RunTitle()
{
	SDL_Event Event;
	while (SDL_PollEvent(&Event) != 0)
	{
		if (Event.type == SDL_QUIT)
		{
			running = false;
		}

		if (Event.key.keysym.sym == SDLK_RETURN && !enterPressed)
		{
			titleTime = 0;
			enterPressed = true;
		}
	}
	frameTime++;
	if (FPS / frameTime <= 15)
	{
		frameTime = 0;
		frameCount++;


		if (frameCount >= 4)
		{
			titleCount++;
			frameCount = 0;
			if (frameRow < 7)
				frameRow++;
		}

		if (titleCount >= 2)
		{
			titleCount = 0;
		}
	}
	if (enterPressed)
		titleTime++;

	if (titleTime >= 80 && enterPressed)
	{
		Mix_PlayMusic(music, 0);
		Mix_VolumeMusic(64);
		posX = 480;
		posY = 338;
		cloudX = 448;
		cloudY = 144;
		bat1X = 286;
		bat1Y = 110;
		bat2X = 0;
		bat2Y = 224;
		gameState = GameState::Intro;
	}
}

void Game::RunIntro()
{
	frameTime++;
	if (FPS / frameTime <= 12)
	{
		frameTime = 0;
		frameCount++;
		if (frameCount >= 4)
			frameCount = 0;
	}
	cloudX -= 0.3f;

	if (introTime < 120)
	{
		bat1X -= 0.2f;
		bat1Y += 0.15f;
	}
	if (introTime > 120 && introTime < 300)
	{
		bat1X -= 0.2f;
		bat1Y -= 0.1f;
	}
	if (introTime > 300 && introTime < 360)
	{
		bat1Y -= 0.2f;
	}

	bat2X += 0.51f;
	bat2Y -= 0.31f;

	introTime++;
	if (posX > 240)
		posX--;

	if (introTime > 420)
		gameState = GameState::Fade;
}






