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
	objects.clear();
	pickups.clear();
	popups.clear();

	levelName = levelID;	

	std::string levelAddress = "Assets/Backgrounds/";

	std::string levelImage = levelID.append(".png");

	levelAddress.append(levelImage);

	tm->load(levelAddress, levelName, renderer);	

	switch (levelN)
	{	
	case 0:
		levelWidth = 1536;	
		tm->load("Assets/Sprites/Misc/wall1.png", "wall1", renderer);
		break;
	case 1:
		levelWidth = 3072;
		break;
	case 2:
		levelWidth = 1024;
		break;
	default:
		break;
	}
	
	levelHeight = 448;
	totalTiles = (levelWidth / TILE_WIDTH) * ((levelHeight - 64) / TILE_HEIGHT);

	setTiles(levelName);

	setPickups(levelName);	

	player.SetLevelArea(levelWidth, levelHeight, totalTiles);

	player.Reset(spawnPointX, spawnPointY);

	score = 0;	
}

void Level::UpdateLevel(SDL_Rect& camera, SoundManager* sm)
{
	if (pause)
		pauseTimer--;

	if (pauseTimer <= 0)
		pause = false;

	if (!pause)
	{
		SpawnEnemies(camera);

		//Iterate through enemies to apply their physics and check for collisions
		std::list<Enemy>::iterator it2 = enemies.begin();
		while (it2 != enemies.end())
		{
			it2->ApplyPhysics();

			if (checkCollision(player.PlayerBox(), it2->EnemyBox()))
			{
				if (player.HitCooldown() <= 0 && !player.PlayerDead())
				{
					player.takeHit(it2->Type());
					sm->play("player_hurt");
				}
			}

			if (checkCollision(player.AttackBox(), it2->EnemyBox()))
			{
				if (it2->getCooldown() <= 0)
				{
					it2->takeHit();
					PopupSprite(it2->EnemyBox().x, player.AttackBox().y - 4, HIT);
					if (it2->HitPoints() <= 0)
					{
						PopupSprite(it2->EnemyBox().x + 8, it2->EnemyBox().y + 16, DEATH);
						it2 = enemies.erase(it2);
						score += 100;						
						sm->play("break");
					}
					/*else if (it2->PosX() < camera.x)
					{
						it2 = enemies.erase(it2);
					}*/
				}
			}
			else
				it2++;
		}

		//Iterate through objects to check for collisions
		std::list<DestroyableObject>::iterator it = objects.begin();
		while (it != objects.end())
		{
			if (checkCollision(player.AttackBox(), it->ObjectBox()))
			{
				switch (it->Type())
				{
				case TORCH:
					sm->play("break");
					PopupSprite(it->ObjectBox().x, player.AttackBox().y - 4, HIT);
					SpawnPickup(it->ObjectBox().x + 16, it->ObjectBox().y, 16);
					PopupSprite(it->ObjectBox().x + 8, it->ObjectBox().y + 16, DEATH);
					break;

				case CANDLE:
					sm->play("break");		
					PopupSprite(it->ObjectBox().x, player.AttackBox().y - 4, HIT);
					SpawnPickup(it->ObjectBox().x + 8, it->ObjectBox().y, 8);
					PopupSprite(it->ObjectBox().x + 4, it->ObjectBox().y + 8, DEATH);
					break;

				default:
					break;

				}
				it = objects.erase(it);
			}
			else
				it++;
		}

		std::list<Pickup>::iterator it3 = pickups.begin();
		while (it3 != pickups.end())
		{
			it3->ApplyPhysics(levelTiles, totalTiles);

			if (checkCollision(player.PlayerBox(), it3->PickupBox()))
			{
				switch (it3->Type())
				{
				case WHIP:
					player.SetWhipLevel(player.WhipLevel() + 1);
					player.SetCurrentAnimation(PlayerAnimation::FLASH);
					pause = true;
					pauseTimer = 60;
					sm->play("whip");
					break;

				case SMALL_HEART:
					player.SetAmmo(player.Ammo() + 1);
					sm->play("heart");
					break;

				case BIG_HEART:
					player.SetAmmo(player.Ammo() + 5);
					sm->play("heart");
					break;

				case RED_MONEYBAG:
					score += 100;
					sm->play("moneybag");
					break;

				case PURPLE_MONEYBAG:
					score += 400;
					sm->play("moneybag");
					break;

				case WHITE_MONEYBAG:
					score += 700;
					sm->play("moneybag");
					break;

				default:
					break;
				}
				it3 = pickups.erase(it3);
			}
			else
				it3++;
		}

		std::list<SpritePopup>::iterator it4 = popups.begin();
		while (it4 != popups.end())
		{
			it4->ApplyPhysics();

			if (it4->Timer() >= 24)
				it4 = popups.erase(it4);
			else
				it4++;
		}

		if (!player.PlayerDead() && !pause)
		{
			player.ApplyPhysics(levelTiles);
			player.PlaySounds(sm);
			player.setCamera(camera);
		}
	}	
}

void Level::DrawLevel(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	//Draw Level background	
	tm->draw(levelName, 0 - camera.x, 0 - camera.y, levelWidth, levelHeight, renderer, SDL_FLIP_NONE);
	
	
	//Draw Objects, iterate through list of objects
	std::list<DestroyableObject>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		it->DrawObject(tm, renderer, camera, frameCount);
	}

	//Draw Enemies, iterate through list of enemies
	std::list<Enemy>::iterator it2;
	for (it2 = enemies.begin(); it2 != enemies.end(); it2++)
	{
		it2->DrawEnemy(tm, renderer, camera, frameCount);
	}

	std::list<Pickup>::iterator it3;
	for (it3 = pickups.begin(); it3 != pickups.end(); it3++)
	{
		it3->DrawPickup(tm, renderer, camera);
	}

	std::list<SpritePopup>::iterator it4;
	for (it4 = popups.begin(); it4 != popups.end(); it4++)
	{
		it4->DrawPopup(tm, renderer, camera, frameCount);
	}

	//Draw Player, switch for various animations
	player.Draw(tm, renderer, camera, frameCount);

	//Piece of wall to cover player on first level
	if (levelName == "level0")
	{
		tm->draw("wall1", 1408 - camera.x, 80 - camera.y, 128, 288, renderer, SDL_FLIP_NONE);
	}	
}

void Level::DrawLevelChange(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount, int doorTimer)
{
	tm->draw(levelName, 0 - camera.x, 0 - camera.y, levelWidth + 512, levelHeight, renderer, SDL_FLIP_NONE);

	if (doorTimer < 384 && doorTimer >= 376 || doorTimer < 296 && doorTimer >= 288)
		tm->drawFrame("door", 3056 - camera.x, 112, 48, 96, 1, 1, renderer, SDL_FLIP_NONE);
	if (doorTimer < 376 && doorTimer >= 296)
		tm->drawFrame("door", 3056 - camera.x, 112, 48, 96, 1, 0, renderer, SDL_FLIP_NONE);

	player.Draw(tm, renderer, camera, frameCount);
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
		DestroyableObject newObject;

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
		case 'T':
			//Torch objects
			newObject.setObject(x - 16, y, ObjectType::TORCH);
			objects.push_back(newObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'C':
			//Candle objects
			newObject.setObject(x - 8, y, ObjectType::CANDLE);
			objects.push_back(newObject);
			levelTiles[i] = new Tile(x, y, '0');
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

		case 'D':
			//Level door
			levelTiles[i] = new Tile(x, y, 'D');
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

void Level::setPickups(std::string levelName)
{
	std::string level = "Assets/Pickups/";

	level.append(levelName.append(".map"));

	std::ifstream map(level);

	for (int i = 0; i < totalTiles; ++i)
	{
		//Determines what kind of tile will be made
		char pickupType = '0';

		//Read tile from map file
		map >> pickupType;

		pickupTiles[i] = pickupType;
	}

	map.close();


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

int Level::Score()
{
	return score;
}

void Level::SpawnEnemies(SDL_Rect& camera)
{
	spawnCooldown--;
	if (levelName == "level1")
	{
		if (player.PosX() < 832 && player.PosX() > 160 && spawnCooldown <= 0)
		{	
			Enemy e1, e2, e3;
			e1.setEnemy(camera.x + 512, 336, EnemyType::GHOUL);
			enemies.push_back(e1);
			e2.setEnemy(camera.x + 576, 336, EnemyType::GHOUL);
			enemies.push_back(e2);
			e3.setEnemy(camera.x + 640, 336, EnemyType::GHOUL);
			enemies.push_back(e3);

			spawnCooldown = 420;
		}
	}
}

void Level::SpawnPickup(int x, int y, int offset)
{

	int width = levelWidth / 32;

	int charLocation = (x / 32) + (((y - 80) / 32) * width);
	char type = pickupTiles[charLocation];

	Pickup newPickup;
	switch (type)
	{
	case '1':
		newPickup.setPickup(x - offset, y, WHIP);
		break;

	case 'S':
		newPickup.setPickup(x - offset, y, SMALL_HEART);
		break;

	case 'B':
		newPickup.setPickup(x - offset, y, BIG_HEART);
		break;

	case 'R':
		newPickup.setPickup(x - offset, y, RED_MONEYBAG);
		break;

	case 'P':
		newPickup.setPickup(x - offset, y, PURPLE_MONEYBAG);
		break;

	case 'W':
		newPickup.setPickup(x - offset, y, WHITE_MONEYBAG);
		break;

	default:
		break;
	}	
	pickups.push_back(newPickup);
}

void Level::PopupSprite(int x, int y, PopupType type)
{
	SpritePopup newPopup;

	newPopup.setPopup(x, y, type);

	popups.push_back(newPopup);
}