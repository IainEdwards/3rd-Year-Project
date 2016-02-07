#include "Level.h"
#include <fstream>

Level::Level()
{	
}

Level::~Level()
{
}

void Level::LoadLevel(std::string levelID, int levelN, TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm)
{
	enemies.clear();	

	levelName = levelID;	

	std::string levelAddress = "Assets/Backgrounds/";

	std::string levelImage = levelID.append(".png");

	levelAddress.append(levelImage);

	tm->load(levelAddress, levelName, renderer);	

	tm->load("Assets/Sprites/Enemies/ghoul.png", "ghoul", renderer);	

	switch (levelN)
	{
	case 0:
		break;
	case 1:
		levelWidth = 1536;	
		tm->load("Assets/Sprites/Misc/wall1.png", "wall1", renderer);
		break;
	case 2:
		levelWidth = 5632;
		break;
	default:
		break;
	}
	
	levelHeight = 448;
	totalTiles = (levelWidth / TILE_WIDTH) * ((levelHeight - 64) / TILE_HEIGHT);

	setTiles(levelName);

	player.Load(tm, renderer, sm);

	player.SetLevelArea(levelWidth, levelHeight, totalTiles);

	player.Reset(spawnPointX, spawnPointY);

	score = 0;

	/*enemy.setPos(1600, 330);
	enemy2.setPos(1630, 310);
	enemy3.setPos(1660, 290);
	enemy4.setPos(1690, 270);
	enemy5.setPos(1720, 250);

	enemies.push_back(enemy);
	enemies.push_back(enemy2);
	enemies.push_back(enemy3);
	enemies.push_back(enemy4);
	enemies.push_back(enemy5);*/

	
}

void Level::UpdateLevel(SDL_Rect& camera)
{

	SpawnEnemies(camera);

	std::list<Enemy>::iterator it = enemies.begin();

	while (it != enemies.end())
	{
		it->ApplyPhysics();
		if (checkCollision(player.AttackBox(), it->EnemyBox()))
		{
			if (it->getCooldown() <= 0)
			{
				it->takeHit();
				it->beginCooldown();
				if (it->HitPoints() <= 0)
				{
					it = enemies.erase(it);
					score += 100;
				}
			}
		}
		else
			it++;
	}

	/*for (it = enemies.begin(); it != enemies.end(); it++)
	{
		it->ApplyPhysics();

		if (checkCollision(player.AttackBox(), it->EnemyBox()))
		{
			if (it->getCooldown() <= 0)
			{
				it->takeHit();
				it->beginCooldown();
				if (it->HitPoints() <= 0)
				{
					enemies.erase(it);
					score += 100;
				}
			}			
		}
	}*/

	//Update enemies, including hits from player
	//for (int i = 0; i < enemies.size(); i++)
	//{
	//	enemies[i].ApplyPhysics();		

	//	if (checkCollision(player.AttackBox(), enemies[i].EnemyBox()))
	//	{
	//		//if (enemies[i].getCooldown() <= 0)
	//		//{
	//			enemies[i].takeHit();
	//			//enemies[i].beginCooldown();
	//			if (enemies[i].HitPoints() <= 0)
	//			{
	//				enemies.erase(enemies.begin() + i);
	//				score += 100;					
	//			}					
	//		//}			
	//	}

	//	if (enemies[i].PosX() <= 0)
	//	{
	//		enemies.erase(enemies.begin() + i);		
	//	}
	//}

	player.ApplyPhysics(levelTiles);

	player.setCamera(camera);
}

void Level::DrawLevel(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	//if need to draw tiles
	//for (int i = 0; i < TOTAL_TILES; ++i)
	//{
		//tm->draw("empty", levelTiles[i]->Box().x - camera.x, levelTiles[i]->Box().y - camera.y, TILE_WIDTH, TILE_HEIGHT, renderer, SDL_FLIP_NONE);
	//}

	//Draw Level background
	tm->draw(levelName, 0 - camera.x, 0 - camera.y, levelWidth, levelHeight, renderer, SDL_FLIP_NONE);

	//Draw Enemies, iterate through vector list of enemies
	std::list<Enemy>::iterator it;
	for (it = enemies.begin(); it != enemies.end(); it++)
	{
		it->DrawEnemy(tm, renderer, camera, frameCount);		
	}

	//Draw Player, switch for various animations
	player.Draw(tm, renderer, camera, frameCount);

	//Piece of wall to cover player on first level
	if (levelName == "level1")
	{
		tm->draw("wall1", 1408 - camera.x, 80 - camera.y, 128, 288, renderer, SDL_FLIP_NONE);
	}
}

bool Level::setTiles(std::string levelName)
{
	//Success flag
	bool tilesLoaded = true;

	//The tile offsets
	int x = 0, y = 80;

	std::string level = "Assets/Levels/";

	level.append(levelName.append(".map"));

	//Open the map
	std::ifstream map(level);

	//Initialize the tiles
	for (int i = 0; i < totalTiles; ++i)
	{
		//Determines what kind of tile will be made
		char tileType = '0';

		//Read tile from map file
		map >> tileType;

		//If the was a problem in reading the map
		if (map.fail())
		{
			//Stop loading map
			printf("Error loading map: Unexpected end of file!\n"); 
			tilesLoaded = false;
			break;
		}

		Enemy newEnemy;

		switch (tileType)
		{
		case '0':
			//empty tile
			levelTiles[i] = new Tile(x, y, tileType);
			break;
		case '1':
			//floor or wall tile
			levelTiles[i] = new Tile(x, y, tileType);
			break;
		case '2':
			//platform tile
			levelTiles[i] = new Tile(x, y, tileType);
			break;
		case 'R':
			//Stair tiles going right and up
			levelTiles[i] = new Tile(x - 16, y, tileType);
			break;
		case 'L':
			//Stair tiles going left and up
			levelTiles[i] = new Tile(x + 16, y, tileType);
			break;
		case 'S':
			//Special case, platform next to top of stair
			levelTiles[i] = new Tile(x, y, tileType);
			break;
		case 'G':			
			newEnemy.setEnemy(x, y - 32, EnemyType::GHOUL);
			enemies.push_back(newEnemy);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'P':
			//Player spawnpoint
			spawnPointX = x - 12;
			spawnPointY = y;
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'X':
			//Level exit
			levelTiles[i] = new Tile(x, y, 'X');
			break;
			
		default:
			break;
				
		}		

		//Move to next tile spot
		x += TILE_WIDTH;

		//If we've gone too far
		if (x >= levelWidth)
		{
			//Move back
			x = 0;

			//Move to the next row
			y += TILE_HEIGHT;
		}
	}

	//Close the file
	map.close();

	//If the map was loaded fine
	return tilesLoaded;
}

bool Level::checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool Level::ReachedExit()
{
	return player.ReachedExit();
}

int Level::Score()
{
	return score;
}

void Level::SpawnEnemies(SDL_Rect& camera)
{
	spawnCooldown--;
	if (levelName == "level2")
	{
		if (player.PosX() < 832 && spawnCooldown <= 0)
		{
			//enemies.resize(enemies.size() + 3);

			Enemy e1, e2, e3;
			e1.setEnemy(camera.x + 512, 336, EnemyType::GHOUL);
			enemies.push_back(e1);
			e2.setEnemy(camera.x + 576, 336, EnemyType::GHOUL);
			enemies.push_back(e2);
			e3.setEnemy(camera.x + 640, 336, EnemyType::GHOUL);
			enemies.push_back(e3);

			spawnCooldown = 360;
		}
	}
}