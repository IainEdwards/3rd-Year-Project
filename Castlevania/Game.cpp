#include <iostream>
#include <fstream>
#include <vector>
#include "SDL_mixer.h"

#include "Game.h"
#include "Player.h"
#include "Level.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "MusicManager.h"

GameState gameState = GameState::Title;

TextureManager *tm = new TextureManager();
SoundManager *sm = new SoundManager();
MusicManager *mm = new MusicManager();

Level levels[5];
int levelN = -1;

int health, lives, ammo, stage, whipLevel, bosshealth;
SubWeaponType subWeaponType;
int totalScore, levelScore, prevScore;
int deathTimer;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Rect camera;

//Variables for title
float frameTime;
int frameCount, frameRow;
int titleTimer, titleCount, titleFrame;
bool enterPressed = false;

//Variables for intro
int introTimer;
int posX, posY = 0;
float cloudX = 0;
int cloudY = 0;
float bat1X, bat1Y = 0;
float bat2X, bat2Y = 0;

//Variables for fade
int fadeTimer;
bool fadeDone;

//Variables for door transition
int doorTimer;
bool doorDone;

bool previousLevel, previousLevel2;
bool secondExit;

//Game time variables
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
	mm->setVolume(64);

	//Allow game loop to start running
	running = true;

	//Load various textures
	LoadAssets();	

	//Initialise the camera
	camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	health = 16;
	bosshealth = 16;
	ammo = 5;
	lives = 3;
	whipLevel = 1;
	totalScore = 0;
	subWeaponType = SubWeaponType::EMPTY;
	previousLevel = false;

	stage = 1;

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

		fadeTimer--;
		if (fadeTimer <= 0)
			fadeDone = true;

		if (fadeDone)
		{		
			levels[5];
			levelN++;	

			if (levelN != 0 && levelN != 1 && levelN != 3 && !previousLevel && !previousLevel2 && !secondExit)
				stage++;

			if (levelN == 0)
				gameTime = 300;

			std::string levelID = "level";
			levelID.append(std::to_string(levelN));

			if (previousLevel)
			{
				levels[levelN].SetPreviousLevelSpawn(true, false);				
			}	
			else if (previousLevel2)
			{
				levels[levelN].SetPreviousLevelSpawn(false, true);
			}
			else
				levels[levelN].SetPreviousLevelSpawn(false, false);

			if (secondExit)
				levels[levelN].SetNextLevelSpawn(true);

			previousLevel = false;
			previousLevel2 = false;
			secondExit = false;
			
			levels[levelN].LoadLevel(levelID, levelN, tm, renderer, sm);						
			levelScore = 0;
			prevScore = totalScore;	
			levels[levelN].player.SetWhipLevel(whipLevel);
			levels[levelN].player.SetHealth(health);
			levels[levelN].player.SetLives(lives);
			levels[levelN].player.SetAmmo(ammo);
			levels[levelN].player.SetSubWeapon(subWeaponType);
			levels[levelN].player.SetAlive();				
			deathTimer = 180;		

			gameState = GameState::Running;
		}
		break;			
	
	case GameState::Running:		
		Update();		
		break;

	case GameState::Door:
		RunDoor();		
		break;

	case GameState::GameOver:
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
			levels[levelN].player.GetInput(Event);			
		}

		//Calculate the frame count for animations
		frameTime++;
		if (FPS / frameTime == 6)
		{
			frameTime = 0;
			frameCount++;
			if (frameCount >= 4)
				frameCount = 0;
		}

		//Update the level
		levels[levelN].UpdateLevel(camera, sm);	

		//Check if music is playing, if not then play
		if (Mix_PlayingMusic() == 0 && !levels[levelN].player.PlayerDead())
		{			
			if (levelN >=0 && levelN <=3)
				mm->play("vampire_killer", 0);
			
		}

		//Calculate change in time for accurate game timer
		currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastUpdate;

		if (deltaTime >= 1000)
		{
			lastUpdate = currentTime;
			gameTime--;
		}

		//Check if the player has reached an exit
		if (levels[levelN].player.ReachedExit())
		{
			fadeTimer = 60;
			fadeDone = false;
			gameState = GameState::Fade;
			SetPlayerStats();
		}

		//Check if the player has reached an exit on stairs
		if (levels[levelN].player.ReachedExitStair())
		{
			fadeTimer = 60;
			fadeDone = false;
			gameState = GameState::Fade;
			SetPlayerStats();
		}

		//Check if the player has reached second exit on stairs in level
		if (levels[levelN].player.ReachedExitStair2())
		{
			fadeTimer = 60;
			fadeDone = false;
			gameState = GameState::Fade;
			SetPlayerStats();
			secondExit = true;
		}

		//Check if the player has reached a door
		if (levels[levelN].player.ReachedDoor())
		{
			doorTimer = 512;
			doorDone = false;
			gameState = GameState::Door;		
			SetPlayerStats();
		}

		//Check if the player has reached an exit to previous level on stairs
		if (levels[levelN].player.ReachedEntrance())
		{
			fadeTimer = 60;
			fadeDone = false;
			gameState = GameState::Fade;
			SetPlayerStats();
			levelN -= 2;
			previousLevel = true;			
		}

		//Check if the player has reached a second exit to previous level on stairs
		if (levels[levelN].player.ReachedEntrance2())
		{
			fadeTimer = 60;
			fadeDone = false;
			gameState = GameState::Fade;
			SetPlayerStats();
			levelN -= 2;
			previousLevel2 = true;
		}

		//Check if the player is dead
		if (levels[levelN].player.PlayerDead())
		{
			if (deathTimer == 180)
			{
				mm->play("player_dead", 1);
			}
			deathTimer--;

			if (deathTimer == 150)
				levels[levelN].player.SetCurrentFrame(1);

			if (deathTimer < 60)
			{
				fadeTimer = 60;
				fadeDone = false;
				subWeaponType = EMPTY;
				levelN--;
				health = 16;
				lives--;
				gameState = GameState::Fade;
			}
		}

		//Calculate the total score
		levelScore = levels[levelN].Score();
		totalScore = levelScore + prevScore;

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
		
		if (!enterPressed)
			tm->drawFrame("titlebat", 368, 192, 144, 112, frameRow, frameCount, renderer, SDL_FLIP_NONE);	

		if (enterPressed)
		{
			tm->drawFrame("titlebat", 368, 192, 144, 112, frameRow, frameCount, renderer, SDL_FLIP_NONE);
			tm->drawFrame("titlecover", 145, 255, 224, 16, 1, titleCount, renderer, SDL_FLIP_NONE);
		}
		break;

	case GameState::Intro:
		//Draw intro screen, player walks across and stops in center, 2 bats and a cloud animated in background
		tm->draw("intro", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);

		if (posX > 240)
			tm->drawFrame("player_introwalk", posX, posY, 32, 64, 1, frameCount, renderer, SDL_FLIP_HORIZONTAL);
		else
			tm->draw("player_intro", posX, posY + 2, 32, 64, renderer, SDL_FLIP_HORIZONTAL);

		tm->draw("introcloud", (int) floor(cloudX), cloudY, 64, 32, renderer, SDL_FLIP_NONE);
		tm->draw("introcover", 480, 128, 32, 64, renderer, SDL_FLIP_NONE);
		tm->drawFrame("introbat", (int)floor(bat1X), (int)floor(bat1Y), 16, 16, 1, frameCount, renderer, SDL_FLIP_NONE);
		tm->drawFrame("introbat", (int)floor(bat2X), (int)floor(bat2Y), 16, 16, 1, frameCount, renderer, SDL_FLIP_NONE);
		break;

	case GameState::Fade:
		//Draw blank screen to transition between levels
		tm->draw("fade", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);
		break;

	case GameState::Running:
		DrawHUD();
		//Draw level, includes player, enemies etc
		levels[levelN].DrawLevel(tm, renderer, camera, frameCount);		

		//Draw the HUD
		//DrawHUD();
		break;

	case GameState::Door:

		DrawHUD();
		//Draw level without enemies and objects
		levels[levelN].DrawLevelChange(tm, renderer, camera, frameCount, doorTimer);	


		//Draw the HUD
		//DrawHUD();
		break;

	case GameState::GameOver:
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
	tm->drawFrame("numbers", 369, 31, 16, 16, 1, levels[levelN].player.Ammo() / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 385, 31, 16, 16, 1, levels[levelN].player.Ammo() % 10, renderer, SDL_FLIP_NONE);

	//Draw lives
	tm->drawFrame("numbers", 369, 47, 16, 16, 1, levels[levelN].player.Lives() / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 385, 47, 16, 16, 1, levels[levelN].player.Lives() % 10, renderer, SDL_FLIP_NONE);

	//Draw stage
	tm->drawFrame("numbers", 465, 15, 16, 16, 1, stage / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 481, 15, 16, 16, 1, stage % 10, renderer, SDL_FLIP_NONE);

	//Draw SubWeapon
	switch (levels[levelN].player.SubWeapon())
	{
	case EMPTY:
		break;
	case DAGGER:
		tm->draw("dagger", 272, 38, 32, 32, renderer, SDL_FLIP_NONE);
		break;

	default:
		break;
	}

	//Draw health
	for (int i = 0; i < levels[levelN].player.Health(); i++)
		tm->draw("health", 112 + i * 8, 34, 8, 12, renderer, SDL_FLIP_NONE);

	//Draw bosshealth
	for (int i = 0; i < bosshealth; i++)
		tm->draw("bosshealth", 112 + i * 8, 50, 8, 12, renderer, SDL_FLIP_NONE);
}

void Game::LoadAssets()
{
	float frameTime = 0;
	int frameCount = 0;

	//Assets for title screen
	tm->load("Assets/Backgrounds/title.png", "title", renderer);
	tm->load("Assets/Sprites/Misc/titlebat.png", "titlebat", renderer);	
	tm->load("Assets/Sprites/Misc/titlecover.png", "titlecover", renderer);
	frameRow = 1;

	//Assets for intro screen
	tm->load("Assets/Backgrounds/intro.png", "intro", renderer);
	tm->load("Assets/Sprites/Player/player_intro.png", "player_intro", renderer);
	tm->load("Assets/Sprites/Player/player_moving.png", "player_introwalk", renderer);
	tm->load("Assets/Sprites/Misc/introcover.png", "introcover", renderer);
	tm->load("Assets/Sprites/Misc/introcloud.png", "introcloud", renderer);
	tm->load("Assets/Sprites/Misc/introbat.png", "introbat", renderer);

	//Assets for fade screen
	tm->load("Assets/Sprites/Misc/fade.png", "fade", renderer);
	fadeDone = false;
	fadeTimer = 60;

	//Assets for door
	tm->load("Assets/Sprites/Misc/door.png", "door", renderer);
	sm->load("Assets/Sounds/door.wav", "door");
	sm->setVolume("door", 32);

	//Assets for HUD
	tm->load("Assets/HUD/hudbase.png", "hudbase", renderer);
	tm->load("Assets/HUD/numbers.png", "numbers", renderer);
	tm->load("Assets/HUD/health.png", "health", renderer);
	tm->load("Assets/HUD/bosshealth.png", "bosshealth", renderer);

	totalScore = 0;
	prevScore = 0;

	//Music assets
	mm->load("Assets/Music/prologue.ogg", "prologue");
	mm->load("Assets/Music/player_dead.ogg", "player_dead");
	mm->load("Assets/Music/vampire_killer.ogg", "vampire_killer");

	//Player assets
	tm->load("Assets/Sprites/Player/player_idle.png", "player_idle", renderer);
	tm->load("Assets/Sprites/Player/player_moving.png", "player_moving", renderer);
	tm->load("Assets/Sprites/Player/player_jumping.png", "player_jumping", renderer);
	tm->load("Assets/Sprites/Player/player_whip1.png", "player_whip1", renderer);
	tm->load("Assets/Sprites/Player/player_whip2.png", "player_whip2", renderer);
	tm->load("Assets/Sprites/Player/player_whip3.png", "player_whip3", renderer);
	tm->load("Assets/Sprites/Player/player_crouching.png", "player_crouching", renderer);
	tm->load("Assets/Sprites/Player/player_crouch_whip1.png", "player_crouch_whip1", renderer);
	tm->load("Assets/Sprites/Player/player_crouch_whip2.png", "player_crouch_whip2", renderer);
	tm->load("Assets/Sprites/Player/player_crouch_whip3.png", "player_crouch_whip3", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_up.png", "player_stairs_up", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_down.png", "player_stairs_down", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_up_whip1.png", "player_stairs_up_whip1", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_up_whip2.png", "player_stairs_up_whip2", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_up_whip3.png", "player_stairs_up_whip3", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_down_whip1.png", "player_stairs_down_whip1", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_down_whip2.png", "player_stairs_down_whip2", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_down_whip3.png", "player_stairs_down_whip3", renderer);
	tm->load("Assets/Sprites/Player/player_damaged.png", "player_damaged", renderer);
	tm->load("Assets/Sprites/Player/player_dead.png", "player_dead", renderer);
	tm->load("Assets/Sprites/Player/player_flash.png", "player_flash", renderer);
	tm->load("Assets/Sprites/Player/player_throwing.png", "player_throwing", renderer);
	tm->load("Assets/Sprites/Player/player_stairs_up_throwing.png", "player_stairs_up_throwing", renderer);

	//Player SFX
	sm->load("Assets/Sounds/player_whip.wav", "player_whip");
	sm->setVolume("player_whip", 32);
	sm->load("Assets/Sounds/player_hurt.wav", "player_hurt");
	sm->setVolume("player_hurt", 48);
	sm->load("Assets/Sounds/player_fall.wav", "player_fall");
	sm->setVolume("player_fall", 32);
	sm->load("Assets/Sounds/player_throw.wav", "player_throw");
	sm->setVolume("player_throw", 32);

	//Sprite popup assets
	tm->load("Assets/Sprites/Misc/hit.png", "hit", renderer);
	tm->load("Assets/Sprites/Misc/death.png", "death", renderer);

	//Enemy assets
	tm->load("Assets/Sprites/Enemies/ghoul.png", "ghoul", renderer);

	//Object assets
	tm->load("Assets/Sprites/Misc/torch.png", "torch", renderer);
	tm->load("Assets/Sprites/Misc/candle.png", "candle", renderer);

	//Item assets
	tm->load("Assets/Sprites/Items/whip.png", "whip", renderer);
	tm->load("Assets/Sprites/Items/small_heart.png", "small_heart", renderer);
	tm->load("Assets/Sprites/Items/big_heart.png", "big_heart", renderer);
	tm->load("Assets/Sprites/Items/red_moneybag.png", "red_moneybag", renderer);
	tm->load("Assets/Sprites/Items/purple_moneybag.png", "purple_moneybag", renderer);
	tm->load("Assets/Sprites/Items/white_moneybag.png", "white_moneybag", renderer);
	tm->load("Assets/Sprites/Items/dagger.png", "dagger", renderer);

	//SFX
	sm->load("Assets/Sounds/break.wav", "break");
	sm->setVolume("break", 32);
	sm->load("Assets/Sounds/weapon.wav", "weapon");
	sm->setVolume("weapon", 32);
	sm->load("Assets/Sounds/heart.wav", "heart");
	sm->setVolume("heart", 32);
	sm->load("Assets/Sounds/moneybag.wav", "moneybag");
	sm->setVolume("moneybag", 32);
	sm->load("Assets/Sounds/enter_castle.wav", "enter_castle");
	sm->setVolume("enter_castle", 32);
	

	
}

bool Game::isRunning()
{
	return running;
}

//Run the title animations
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
			titleTimer = 0;
			enterPressed = true;			
		}
	}

	frameTime++;
	if (FPS / frameTime <= 15 && !enterPressed)
	{
		frameTime = 0;
		frameCount++;
		if (frameCount >= 4)
		{			
			frameCount = 0;
			if (frameRow < 7)
				frameRow++;
		}		
	}

	if (enterPressed)
	{
		titleTimer++;
		if (titleTimer % 10 == 0)
		{
			titleCount++;			
		}
		if (titleCount >= 2)
		{
			titleCount = 0;
		}
	}

	if (titleTimer >= 80 && enterPressed)
	{
		mm->play("prologue", 0);
		mm->setVolume(64);
		
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

//Run the intro animations 
void Game::RunIntro()
{
	frameTime++;
	if (FPS / frameTime <= 8)
	{
		frameTime = 0;
		frameCount++;
		if (frameCount >= 4)
			frameCount = 0;
	}
	cloudX -= 0.3f;

	if (introTimer < 120)
	{
		bat1X -= 0.2f;
		bat1Y += 0.15f;
	}
	if (introTimer > 120 && introTimer < 300)
	{
		bat1X -= 0.2f;
		bat1Y -= 0.1f;
	}
	if (introTimer > 300 && introTimer < 360)
	{
		bat1Y -= 0.2f;
	}

	bat2X += 0.51f;
	bat2Y -= 0.31f;

	introTimer++;
	if (posX > 240)
		posX--;

	if (introTimer > 420)
	{
		gameState = GameState::Fade;		
	}
		
}

//Run the door animations and change to next level
void Game::RunDoor()
{
	doorTimer--;
	if (doorTimer <= 0)
		doorDone = true;

	if (doorTimer >= 384)
	{
		camera.x += 2;
		frameCount = 0;
	}

	if (doorTimer == 384 || doorTimer == 296)
	{
		sm->play("door");
	}

	if (doorTimer < 368 && doorTimer >= 296)
	{
		frameTime++;
		if (FPS / frameTime == 6)
		{
			frameTime = 0;
			frameCount++;
			if (frameCount >= 4)
				frameCount = 0;
		}
		levels[levelN].player.ChangePosX(2);
	}
	if (doorTimer < 296)
		frameCount = 0;

	if (doorTimer < 288 && doorTimer >= 160)
	{
		camera.x += 2;

	}

	if (doorTimer < 160)
		doorDone = true;

	if (doorDone)
	{
		levels[levelN];
		if (levelN == 2)
			levelN += 2;
		else
			levelN++;


		std::string levelID = "level";
		levelID.append(std::to_string(levelN));

		levels[levelN].LoadLevel(levelID, levelN, tm, renderer, sm);
		stage++;
		levelScore = 0;
		prevScore = totalScore;		
		levels[levelN].player.SetWhipLevel(whipLevel);
		levels[levelN].player.SetHealth(health);
		levels[levelN].player.SetLives(lives);
		levels[levelN].player.SetAmmo(ammo);
		levels[levelN].player.SetSubWeapon(subWeaponType);
		levels[levelN].player.SetAlive();
		deathTimer = 180;
		camera.x = 0;
		camera.y = 0;
		gameState = GameState::Running;
	}
}

//Set the player stats for next level load
void Game::SetPlayerStats()
{
	health = levels[levelN].player.Health();
	lives = levels[levelN].player.Lives();
	ammo = levels[levelN].player.Ammo();
	subWeaponType = levels[levelN].player.SubWeapon();
	whipLevel = levels[levelN].player.WhipLevel();
}






