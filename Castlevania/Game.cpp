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

Level levels[20];
int levelN = -1;

int health, lives, ammo, stage, whipLevel, maxProjectiles;
SubWeaponType subWeaponType;
int totalScore, prevScore;
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

bool changeMusic;

bool previousLevel, previousLevel2;
bool secondExit;

//Variables for levelComplete
int levelCompleteTimer;
int addScoreTimer;
bool heartScore;
bool goToMap;

//Variables for map
int mapX;
int playerX;

//Variables for game over
int heartY;
int selectTimer;
bool gameoverMusic;

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
	ammo = 5;
	lives = 3;
	whipLevel = 1;
	totalScore = 0;
	subWeaponType = SubWeaponType::EMPTY;
	previousLevel = false;
	previousLevel2 = false;
	secondExit = false;
	stage = 1;
	maxProjectiles = 1;

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
			levels[10];
			levelN++;	

			switch (levelN)
			{
			//First main level
			case 0:
				stage = 1;
				gameTime = 300;
				break;
			case 1:
				stage = 1;
				break;
			case 2:
				stage = 2;
				break;
			case 3:
				stage = 2;
				break;
			case 4:
				stage = 3;
				break;

			//Second main level
			case 5:
				stage = 4;
				gameTime = 400;
				break;
			case 6:
				stage = 4;
				break;
			case 7:
				stage = 5;				
				break;
			case 8:
				stage = 5;
				break;
			case 9:
				stage = 6;
				break;
			case 10:
				stage = 6;
				break;

			//Third main level
			case 11:
				stage = 7;
				break;
			default:
				break;
			}

			if (levelN == 0 || levelN == 1)
			{
				stage = 1;
			}

			if (levelN == 0 || levelN == 1)
				gameTime = 300;

			if (levelN == 5)
				gameTime = 400;

			if (levelN == 11)
				gameTime = 500;

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
			levels[levelN].SetScore(0);
			prevScore = totalScore;	
			levels[levelN].player.SetWhipLevel(whipLevel);
			levels[levelN].player.SetHealth(health);
			levels[levelN].player.SetLives(lives);
			levels[levelN].player.SetAmmo(ammo);
			levels[levelN].player.SetMaxProjectiles(maxProjectiles);
			levels[levelN].player.SetSubWeapon(subWeaponType);
			levels[levelN].player.SetAlive();				
			deathTimer = 180;		
			changeMusic = true;

			if (levelN == 1 || levelN == 2 || levelN == 4 || levelN == 5 || levelN == 8)
				camera.x = 0;

			if (levelN == 6)
				camera.x = 512;

			if (levelN == 7 || levelN == 9 || levelN == 10 || levelN == 11)
				camera.x = 1024;

			

			gameState = GameState::Running;
		}
		break;			
	
	case GameState::Running:		
		Update();		
		break;

	case GameState::Door:
		RunDoor();		
		break;

	case GameState::LevelComplete:
		RunLevelComplete();
		break;

	case GameState::Map:
		RunMap();
		break;

	case GameState::GameOver:
		RunGameOver();
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

		//Calculate the total score		
		totalScore = prevScore + levels[levelN].Score();

		//Handle events on queue
		while (SDL_PollEvent(&Event) != 0)
		{
			//User requests quit
			if (Event.type == SDL_QUIT)
			{
				running = false;
			}			

			//Handle input for the player
			if (!levels[levelN].player.PlayerDead() && !levels[levelN].Pause())
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

		//Check if boss fight, to change music
		if (levels[levelN].player.BossFight() && changeMusic)
		{
			Mix_FadeOutMusic(2000);
			changeMusic = false;
		}

		//Check if music is playing, if not then play
		if (Mix_PlayingMusic() == 0 && !levels[levelN].player.PlayerDead() && !levels[levelN].player.BossFight())
		{			
			if (levelN >= 0 && levelN <= 4)
				mm->play("vampire_killer", 0);
			else if (levelN >= 5 && levelN <= 10)
				mm->play("stalker", 0);
			else if (levelN >= 11 && levelN <= 16)
				mm->play("wicked_child", 0);
		}
		else if (Mix_PlayingMusic() == 0 && levels[levelN].player.BossFight())
		{
			mm->play("poison_mind", 0);
		}

		//Calculate change in time for accurate game timer
		currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastUpdate;

		if (deltaTime >= 1000 && !levels[levelN].player.PlayerDead() )
		{
			lastUpdate = currentTime;

			if (!levels[levelN].Stopwatch())
				gameTime--;

			if (gameTime <= 30 && gameTime > 0)
			{
				sm->play("alarm");
			}
			if (gameTime <= 0)
			{
				levels[levelN].player.playerDeath();
			}
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
				if (lives == 0)
				{
					heartY = 272;
					gameoverMusic = false;
					selectTimer = 0;
					gameState = GameState::GameOver;
				}
				else
				{
					fadeTimer = 60;
					fadeDone = false;
					subWeaponType = EMPTY;
					if (stage == 1)
					{
						gameTime = 300;
						levelN = 0;
					}
					else if (stage == 2)
					{
						gameTime = 200;
						levelN = 1;
					}
					else if (stage == 3)
					{
						gameTime = 100;
						levelN = 3;
					}
					else if (stage == 4)
					{
						gameTime = 400;
						levelN = 4;
					}
					else if (stage == 5)
					{
						gameTime = 300;
						levelN = 6;
					}
					else if (stage == 6)
					{
						gameTime = 200;
						levelN = 8;
					}
					health = 16;
					lives--;
					maxProjectiles = 1;
					gameState = GameState::Fade;
				}
			}
		}

		if (levels[levelN].LevelComplete())
		{
			Mix_HaltMusic();
			mm->play("stage_clear", 0);
			levelCompleteTimer = 0;
			heartScore = false;
			goToMap = false;
			ammo = 5;
			subWeaponType = levels[levelN].player.SubWeapon();			
			whipLevel = levels[levelN].player.WhipLevel();
			maxProjectiles = levels[levelN].player.MaxProjectiles();
			gameState = GameState::LevelComplete;
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
		//Draw the HUD
		DrawHUD();

		//Draw level, includes player, enemies etc
		levels[levelN].DrawLevel(tm, renderer, camera, frameCount);		
		break;

	case GameState::Door:
		//Draw the HUD
		DrawHUD();
		//Draw level without enemies and objects
		levels[levelN].DrawLevelChange(tm, renderer, camera, frameCount, doorTimer);	
		break;

	case GameState::LevelComplete:
		DrawHUD();

		levels[levelN].DrawLevel(tm, renderer, camera, frameCount);

		if (goToMap && levelCompleteTimer > 60)
			tm->draw("fade", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);
		break;

	case GameState::Map:
		
		tm->draw("fade", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);
		tm->draw("map", mapX, 0, 784, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);

		if (mapX == 0)
		{
			if (levelN == 4)
			{
				tm->drawFrame("map_bat", 182, 214, 32, 32, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
				tm->drawFrame("map_mark", 302, 274, 32, 32, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
				tm->drawFrame("player_introwalk", playerX, 306, 32, 64, 1, frameCount, renderer, SDL_FLIP_NONE);
			}
			if (levelN == 10)
			{
				tm->drawFrame("map_bat", 342, 183, 32, 32, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
				tm->drawFrame("map_mark", 158, 216, 32, 32, 1, frameCount % 2, renderer, SDL_FLIP_NONE);
				tm->drawFrame("player_introwalk", playerX, 306, 32, 64, 1, frameCount, renderer, SDL_FLIP_NONE);
			}
		}

		DrawHUD();
		break;


	case GameState::GameOver:

		tm->draw("game_over", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer, SDL_FLIP_NONE);

		tm->draw("game_over_heart", 176, heartY, 16, 16, renderer, SDL_FLIP_NONE);


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
	if (gameState != GameState::Map)
	{
		tm->drawFrame("numbers", 369, 31, 16, 16, 1, levels[levelN].player.Ammo() / 10 % 10, renderer, SDL_FLIP_NONE);
		tm->drawFrame("numbers", 385, 31, 16, 16, 1, levels[levelN].player.Ammo() % 10, renderer, SDL_FLIP_NONE);
	}
	else if (gameState = GameState::Map)
	{
		tm->drawFrame("numbers", 369, 31, 16, 16, 1, 0, renderer, SDL_FLIP_NONE);
		tm->drawFrame("numbers", 385, 31, 16, 16, 1, 5, renderer, SDL_FLIP_NONE);
	}
	

	//Draw lives
	tm->drawFrame("numbers", 369, 47, 16, 16, 1, lives / 10 % 10, renderer, SDL_FLIP_NONE);
	tm->drawFrame("numbers", 385, 47, 16, 16, 1, lives % 10, renderer, SDL_FLIP_NONE);

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
	case STOPWATCH:
		tm->draw("stopwatch", 272, 38, 32, 32, renderer, SDL_FLIP_NONE);
		break;
	case HOLY_WATER:
		tm->draw("holy_water", 272, 38, 32, 32, renderer, SDL_FLIP_NONE);
		break;
	case AXE:
		tm->draw("axe", 272, 38, 32, 32, renderer, SDL_FLIP_NONE);
		break;
	case CROSS:
		tm->draw("cross", 272, 38, 32, 32, renderer, SDL_FLIP_NONE);
		break;
	default:
		break;
	}

	//Draw health
	for (int i = 0; i < levels[levelN].player.Health(); i++)
		tm->draw("health", 112 + i * 8, 34, 8, 12, renderer, SDL_FLIP_NONE);

	//Draw bosshealth
	if (stage == 3)
		for (int i = 0; i < levels[levelN].levelBoss.Health(); i++)
			tm->draw("bosshealth", 112 + i * 8, 50, 8, 12, renderer, SDL_FLIP_NONE);
	else if (levelN == 10)
		for (int i = 0; i < levels[levelN].levelBoss.Health(); i+=2)
			tm->draw("bosshealth", 112 + i * 4, 50, 8, 12, renderer, SDL_FLIP_NONE);
	else if (gameState != GameState::Map)
		for (int i = 0; i < 16; i++)
			tm->draw("bosshealth", 112 + i * 8, 50, 8, 12, renderer, SDL_FLIP_NONE);

	//Draw double/triple shot
	if (levels[levelN].player.MaxProjectiles() == 2)
		tm->draw("double_shot", 418, 34, 32, 32, renderer, SDL_FLIP_NONE);

	if (levels[levelN].player.MaxProjectiles() == 3)
		tm->draw("triple_shot", 418, 34, 32, 32, renderer, SDL_FLIP_NONE);
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

	//Assets for map screen
	tm->load("Assets/Backgrounds/map.png", "map", renderer);
	tm->load("Assets/Sprites/Misc/map_bat.png", "map_bat", renderer);
	tm->load("Assets/Sprites/Misc/map_mark.png", "map_mark", renderer);

	//Assets for game over screen
	tm->load("Assets/Backgrounds/game_over.png", "game_over", renderer);
	tm->load("Assets/Sprites/Misc/game_over_heart.png", "game_over_heart", renderer);

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
	mm->load("Assets/Music/stalker.ogg", "stalker");
	mm->load("Assets/Music/poison_mind.ogg", "poison_mind");
	mm->load("Assets/Music/stage_clear.ogg", "stage_clear");
	mm->load("Assets/Music/game_over.ogg", "game_over");
	mm->load("Assets/Music/wicked_child.ogg", "wicked_child");

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
	tm->load("Assets/Sprites/Misc/splash.png", "splash", renderer);
	tm->load("Assets/Sprites/Misc/100.png", "100", renderer);
	tm->load("Assets/Sprites/Misc/400.png", "400", renderer);
	tm->load("Assets/Sprites/Misc/700.png", "700", renderer);
	tm->load("Assets/Sprites/Misc/debris.png", "debris", renderer);

	//Enemy assets
	tm->load("Assets/Sprites/Enemies/ghoul.png", "ghoul", renderer);
	tm->load("Assets/Sprites/Enemies/panther_idle.png", "panther_idle", renderer);
	tm->load("Assets/Sprites/Enemies/panther_running.png", "panther_running", renderer);
	tm->load("Assets/Sprites/Enemies/panther_jumping.png", "panther_jumping", renderer);
	tm->load("Assets/Sprites/Enemies/bat_idle.png", "bat_idle", renderer);
	tm->load("Assets/Sprites/Enemies/bat_flying.png", "bat_flying", renderer);
	tm->load("Assets/Sprites/Enemies/bat_idle_blue.png", "bat_idle_blue", renderer);
	tm->load("Assets/Sprites/Enemies/bat_flying_blue.png", "bat_flying_blue", renderer);
	tm->load("Assets/Sprites/Enemies/fishman_idle.png", "fishman_idle", renderer);
	tm->load("Assets/Sprites/Enemies/fishman_moving.png", "fishman_moving", renderer);
	tm->load("Assets/Sprites/Enemies/fishman_shooting.png", "fishman_shooting", renderer);
	tm->load("Assets/Sprites/Enemies/spear_knight.png", "spear_knight", renderer);
	tm->load("Assets/Sprites/Enemies/medusa_head.png", "medusa_head", renderer);
	tm->load("Assets/Sprites/Enemies/bone_tower.png", "bone_tower", renderer);
	tm->load("Assets/Sprites/Enemies/snake.png", "snake", renderer);
	tm->load("Assets/Sprites/Misc/projectile.png", "projectile", renderer);

	//Object assets
	tm->load("Assets/Sprites/Misc/torch.png", "torch", renderer);
	tm->load("Assets/Sprites/Misc/candle.png", "candle", renderer);
	tm->load("Assets/Sprites/Misc/tile1.png", "tile1", renderer);
	tm->load("Assets/Sprites/Misc/tile2.png", "tile2", renderer);
	tm->load("Assets/Sprites/Misc/tile3.png", "tile3", renderer);
	tm->load("Assets/Sprites/Misc/tile4.png", "tile4", renderer);
	tm->load("Assets/Sprites/Misc/platform.png", "platform", renderer);
	tm->load("Assets/Sprites/Misc/crusher.png", "crusher", renderer);
	tm->load("Assets/Sprites/Misc/crusher2.png", "crusher2", renderer);
	tm->load("Assets/Sprites/Misc/crushercover.png", "crushercover", renderer);

	//Boss assets
	tm->load("Assets/Sprites/Bosses/vampire_bat.png", "vampire_bat", renderer);
	tm->load("Assets/Sprites/Bosses/vampire_bat_idle.png", "vampire_bat_idle", renderer);
	tm->load("Assets/Sprites/Bosses/medusa.png", "medusa", renderer);
	tm->load("Assets/Sprites/Bosses/medusa_idle.png", "medusa_idle", renderer);

	//Item assets
	tm->load("Assets/Sprites/Items/whip.png", "whip", renderer);
	tm->load("Assets/Sprites/Items/small_heart.png", "small_heart", renderer);
	tm->load("Assets/Sprites/Items/big_heart.png", "big_heart", renderer);
	tm->load("Assets/Sprites/Items/red_moneybag.png", "red_moneybag", renderer);
	tm->load("Assets/Sprites/Items/purple_moneybag.png", "purple_moneybag", renderer);
	tm->load("Assets/Sprites/Items/white_moneybag.png", "white_moneybag", renderer);
	tm->load("Assets/Sprites/Items/dagger.png", "dagger", renderer);
	tm->load("Assets/Sprites/Items/stopwatch.png", "stopwatch", renderer);
	tm->load("Assets/Sprites/Items/holy_water.png", "holy_water", renderer);
	tm->load("Assets/Sprites/Misc/holy_water_flame.png", "holy_water_flame", renderer);
	tm->load("Assets/Sprites/Items/axe.png", "axe", renderer);
	tm->load("Assets/Sprites/Items/cross.png", "cross", renderer);
	tm->load("Assets/Sprites/Items/rosary.png", "rosary", renderer);
	tm->load("Assets/Sprites/Misc/flash.png", "flash", renderer);
	tm->load("Assets/Sprites/Items/roast.png", "roast", renderer);
	tm->load("Assets/Sprites/Items/double_shot.png", "double_shot", renderer);
	tm->load("Assets/Sprites/Items/double_shot_drop.png", "double_shot_drop", renderer);
	tm->load("Assets/Sprites/Items/triple_shot.png", "triple_shot", renderer);
	tm->load("Assets/Sprites/Items/triple_shot_drop.png", "triple_shot_drop", renderer);
	tm->load("Assets/Sprites/Items/spirit_ball.png", "spirit_ball", renderer);

	//SFX
	sm->load("Assets/Sounds/break.wav", "break");
	sm->setVolume("break", 32);
	sm->load("Assets/Sounds/break_tile.wav", "break_tile");
	sm->setVolume("break_tile", 32);
	sm->load("Assets/Sounds/weapon.wav", "weapon");
	sm->setVolume("weapon", 32);
	sm->load("Assets/Sounds/heart.wav", "heart");
	sm->setVolume("heart", 32);
	sm->load("Assets/Sounds/moneybag.wav", "moneybag");
	sm->setVolume("moneybag", 32);
	sm->load("Assets/Sounds/enter_castle.wav", "enter_castle");
	sm->setVolume("enter_castle", 32);
	sm->load("Assets/Sounds/time_score.wav", "time_score");
	sm->setVolume("time_score", 16);
	sm->load("Assets/Sounds/heart_score.wav", "heart_score");
	sm->setVolume("heart_score", 16);
	sm->load("Assets/Sounds/player_moving.wav", "player_moving");
	sm->setVolume("player_moving", 32);
	sm->load("Assets/Sounds/game_over_heart.wav", "game_over_heart");
	sm->setVolume("game_over_heart", 32);
	sm->load("Assets/Sounds/splash_up.wav", "splash_up");
	sm->setVolume("splash_up", 32);
	sm->load("Assets/Sounds/splash_down.wav", "splash_down");
	sm->setVolume("splash_down", 32);
	sm->load("Assets/Sounds/alarm.wav", "alarm");
	sm->setVolume("alarm", 32);
	sm->load("Assets/Sounds/stopwatch.wav", "stopwatch");
	sm->setVolume("stopwatch", 32);
	sm->load("Assets/Sounds/holy_water.wav", "holy_water");
	sm->setVolume("holy_water", 32);
	sm->load("Assets/Sounds/rosary.wav", "rosary");
	sm->setVolume("rosary", 32);
	sm->load("Assets/Sounds/cross.wav", "cross");
	sm->setVolume("cross", 32);
	sm->load("Assets/Sounds/enemy_hit.wav", "enemy_hit");
	sm->setVolume("enemy_hit", 32);
	sm->load("Assets/Sounds/extra_life.wav", "extra_life");
	sm->setVolume("extra_life", 32);
	
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
	int doorMove = 1;

	if (levelN == 1 || levelN == 2)	
		doorMove = 1;	
	else
		doorMove = -1;

	doorTimer--;
	if (doorTimer <= 0)
		doorDone = true;

	if (doorTimer >= 384)
	{
		camera.x += 2 * doorMove;
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
		levels[levelN].player.ChangePosX(2 * doorMove);
	}
	if (doorTimer < 296)
		frameCount = 0;

	if (doorTimer < 288 && doorTimer >= 160)
	{
		camera.x += 2 * doorMove;

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
		levels[levelN].SetScore(0);
		prevScore = totalScore;
		levels[levelN].player.SetWhipLevel(whipLevel);
		levels[levelN].player.SetHealth(health);
		levels[levelN].player.SetLives(lives);
		levels[levelN].player.SetAmmo(ammo);
		levels[levelN].player.SetMaxProjectiles(maxProjectiles);
		levels[levelN].player.SetSubWeapon(subWeaponType);
		levels[levelN].player.SetAlive();
		deathTimer = 180;
		changeMusic = true;

		if (levelN == 1 || levelN == 2 || levelN == 4)
			camera.x = 0;			
		
		if (levelN == 7 || levelN == 9)		
			camera.x = 1024;		
		
		gameState = GameState::Running;
	}
}

//Run the level complete sequence
void Game::RunLevelComplete()
{
	levelCompleteTimer++;
	addScoreTimer++;

	if (levels[levelN].player.Health() < 16 && levelCompleteTimer % 10 == 0)
	{
		levels[levelN].player.SetHealth(levels[levelN].player.Health() + 1);
	}

	if (levelCompleteTimer > 240 && !heartScore)
	{
		if (gameTime > 1)
		{					
			gameTime--;
			totalScore += 10;					
		}
		if (gameTime <= 1)
		{			
			totalScore += 10;
			gameTime = 0;	
			heartScore = true;
			levelCompleteTimer = 0;
		}
		if (addScoreTimer > 7)
		{
			Mix_HaltChannel(-1);
			sm->play("time_score");
			addScoreTimer = 0;
		}
	}

	if (levelCompleteTimer > 60 && heartScore && !goToMap)
	{
		if (levels[levelN].player.Ammo() >= 1 && addScoreTimer > 8)
		{
			Mix_HaltChannel(-1);
			sm->play("heart_score");
			levels[levelN].player.SetAmmo(levels[levelN].player.Ammo() - 1);
			totalScore += 100;
			addScoreTimer = 0;
		}
		if (levels[levelN].player.Ammo() >= 0 && addScoreTimer > 8)
		{						
			levels[levelN].player.SetAmmo(0);
			addScoreTimer = 0;				
			goToMap = true;
			levelCompleteTimer = 0;
		}
	}

	if (levelCompleteTimer > 120 && goToMap)
	{
		stage += 1;
		mapX = 512;
		playerX = 0;
		gameState = GameState::Map;
	}
}

//Run the map screen 
void Game::RunMap()
{
	frameTime++;
	if (FPS / frameTime == 6)
	{
		frameTime = 0;
		frameCount++;
		if (frameCount >= 4)
			frameCount = 0;
	}

	if (mapX > 0)
		mapX -= 4;
	else if (mapX < 0)
		mapX = 0;

	if (mapX == 0)
	{
		playerX += 2;
		if (playerX % 40 == 0)
		{
			Mix_HaltChannel(-1);
			sm->play("player_moving");
		}		
	}	

	if (playerX >= 480)
	{
		fadeTimer = 60;
		fadeDone = false;
		gameState = GameState::Fade;
	}
}

//Run the game over screen
void Game::RunGameOver()
{
	if (!gameoverMusic)
	{	
		Mix_HaltMusic();
		mm->play("game_over", 0);
		gameoverMusic = true;
	}

	selectTimer++;

	SDL_Event Event;
	while (SDL_PollEvent(&Event) != 0)
	{
		if (Event.key.keysym.sym == SDLK_DOWN && selectTimer > 15)
		{
			if (heartY == 272)
			{
				heartY = 320;
				sm->play("game_over_heart");
				selectTimer = 0;
			}
			else if (heartY == 320)
			{
				heartY = 272;
				sm->play("game_over_heart");
				selectTimer = 0;
			}
		}
		if (Event.key.keysym.sym == SDLK_UP && selectTimer > 15)
		{
			if (heartY == 272)
			{
				heartY = 320;
				sm->play("game_over_heart");
				selectTimer = 0;
			}
			else if (heartY == 320)
			{
				heartY = 272;
				sm->play("game_over_heart");
				selectTimer = 0;
			}
		}
		if (Event.key.keysym.sym == SDLK_RETURN && selectTimer > 15)
		{
			if (heartY == 272)
			{
				if (stage >= 1 && stage <= 3)
				{
					health = 16;
					ammo = 5;
					lives = 3;
					whipLevel = 1;
					totalScore = 0;
					subWeaponType = SubWeaponType::EMPTY;
					previousLevel = false;
					previousLevel2 = false;
					secondExit = false;
					stage = 1;
					levelN = 0;
					fadeTimer = 60;
					fadeDone = false;
					gameState = GameState::Fade;
				}
				else if (stage >= 4 && stage <= 6)
				{
					health = 16;
					ammo = 5;
					lives = 3;
					whipLevel = 1;
					totalScore = 0;
					subWeaponType = SubWeaponType::EMPTY;
					previousLevel = false;
					previousLevel2 = false;
					secondExit = false;
					stage = 1;
					levelN = 4;
					fadeTimer = 60;
					fadeDone = false;
					gameState = GameState::Fade;
				}
			}
			else if (heartY == 320)
			{
				running = false;
			}
		}
	}
}

//Set the player stats for next level load
void Game::SetPlayerStats()
{		
	health = levels[levelN].player.Health();
	lives = levels[levelN].player.Lives();
	ammo = levels[levelN].player.Ammo();
	subWeaponType = levels[levelN].player.SubWeapon();
	maxProjectiles = levels[levelN].player.MaxProjectiles();
	whipLevel = levels[levelN].player.WhipLevel();
	
}






