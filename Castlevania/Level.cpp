#include "Level.h"
#include <fstream>

Level::Level()
{	
}

Level::~Level()
{
}

void Level::SetPreviousLevelSpawn(bool spawnOne, bool spawnTwo)
{
	previousLevel = spawnOne;
	previousLevel2 = spawnTwo;
}

void Level::SetNextLevelSpawn(bool spawnTwo)
{
	secondExit = spawnTwo;
}

void Level::LoadLevel(std::string levelID, int levelN, TextureManager* tm, SDL_Renderer* renderer, SoundManager* sm)
{
	enemies.clear();
	objects.clear();
	pickups.clear();
	popups.clear();
	projectiles.clear();

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
	case 3:
		levelWidth = 1024;
		break;	

	case 4:
		levelWidth = 1536;
		break;

	case 5:
		levelWidth = 512;
		break;

	default:
		break;
	}
	
	levelHeight = 448;
	totalTiles = (levelWidth / TILE_WIDTH) * ((levelHeight - 64) / TILE_HEIGHT);

	setTiles(levelName);

	setPickups(levelName);	

	player.SetLevelArea(levelWidth, levelHeight, totalTiles);

	if (!previousLevel && !previousLevel2 && !secondExit)
		player.Reset(spawnPointX, spawnPointY);
	else if (!previousLevel && !previousLevel2 && secondExit)
		player.Reset(spawnPoint2X, spawnPoint2Y);
	else if (previousLevel && !previousLevel2)
		player.Reset(spawnPoint2X, spawnPoint2Y);
	else if (!previousLevel && previousLevel2)
		player.Reset(spawnPoint3X, spawnPoint3Y);

	if (levelN == 3)			
		player.SetOnStairs(false);
		
	if (levelN == 2 && previousLevel || levelN == 2 && previousLevel2)
		player.SetOnStairs(true);

	score = 0;	
	previousLevel = false;
	previousLevel2 = false;
	secondExit = false;

	levelComplete = false;

	playerSplash = true;
}

void Level::UpdateLevel(SDL_Rect& camera, SoundManager* sm)
{
	if (pause)
		pauseTimer--;

	if (pauseTimer <= 0)
		pause = false;

	if (!pause)
	{
		if (player.SpawnProjectile())
		{

			if (!player.Flip())
				SpawnProjectile(player.PosX(), player.PosY(), 1, player.SubWeapon());
			else
				SpawnProjectile(player.PosX(), player.PosY(), -1, player.SubWeapon());
		}

		SpawnEnemies(camera);

		//Iterate through enemies to apply their physics and check for collisions
		std::list<Enemy>::iterator it2 = enemies.begin();			
		while (it2 != enemies.end())
		{
			it2->ApplyPhysics(levelTiles, totalTiles);

			if (checkCollision(player.PlayerBox(), it2->EnemyBox()))
			{
				if (player.HitCooldown() <= 0 && !player.PlayerDead())
				{
					if (player.PlayerBox().x < it2->EnemyBox().x)
						player.takeHit(it2->Type(), false);
					else
						player.takeHit(it2->Type(), true);

					sm->play("player_hurt");
				}
			}	

			if (it2->Type() == PANTHER && it2->EnemyBox().x - player.PlayerBox().x < 128)
			{
				it2->SetIdle(false);
			}

			if (checkCollision(player.AttackBox(), it2->EnemyBox()))
			{
				if (it2->getCooldown() <= 0)
				{
					it2->takeHit(player.WhipLevel());
					PopupSprite(it2->EnemyBox().x, player.AttackBox().y - 4, HIT);
					if (it2->HitPoints() <= 0)
					{
						PopupSprite(it2->EnemyBox().x + (it2->EnemyBox().w / 2) - 8, 
									it2->EnemyBox().y + (it2->EnemyBox().h / 2) - 16, DEATH);
						it2 = enemies.erase(it2);
						score += 100;						
						sm->play("break");
					}					
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
					sm->play("weapon");
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
					PopupSprite(it3->PickupBox().x, it3->PickupBox().y, RED_POINTS);
					sm->play("moneybag");
					break;

				case PURPLE_MONEYBAG:
					score += 400;
					PopupSprite(it3->PickupBox().x, it3->PickupBox().y, PURPLE_POINTS);
					sm->play("moneybag");
					break;

				case WHITE_MONEYBAG:
					score += 700;
					PopupSprite(it3->PickupBox().x, it3->PickupBox().y, WHITE_POINTS);
					sm->play("moneybag");
					break;

				case DAGGER_DROP:
					sm->play("weapon");
					player.SetSubWeapon(SubWeaponType::DAGGER);
					break;

				case SPIRIT_BALL:
					if (it3->PickupTimer() > 120)
						levelComplete = true;
					break;


				default:
					break;
				}
				if (it3->Type() == PickupType::SPIRIT_BALL)
				{
					if (it3->PickupTimer() > 120)
						it3 = pickups.erase(it3);
					else
						it3++;
				}
				else
					it3 = pickups.erase(it3);
			}
			else
				it3++;
		}

		std::list<SpritePopup>::iterator it4 = popups.begin();
		while (it4 != popups.end())
		{
			it4->ApplyPhysics();

			if (it4->Timer() >= 24 && it4->Type() != SPLASH_LEFT && it4->Type() != SPLASH_RIGHT)
				it4 = popups.erase(it4);
			else if (it4->Timer() >= 60)
				it4 = popups.erase(it4);
			else
				it4++;
		}

		std::list<SubWeapon>::iterator it5 = projectiles.begin();		
		while (it5 != projectiles.end())
		{
			it5->ApplyPhysics();			

			if (it5->SubWeaponBox().x < camera.x || it5->SubWeaponBox().x > camera.x + 512)
			{
				it5 = projectiles.erase(it5);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);				
			}
			else
				it5++;
		}

		//Check collision between projectiles and enemies
		for (std::list<SubWeapon>::iterator i = projectiles.begin(); i != projectiles.end();)
		{
			for (std::list<Enemy>::iterator j = enemies.begin(); j != enemies.end();) 
			{
				if (checkCollision(i->SubWeaponBox(), j->EnemyBox()))
				{

					j->takeHit(player.WhipLevel());
					PopupSprite(j->EnemyBox().x, i->SubWeaponBox().y + 16, HIT);
					i->SetCooldown(60);
					if (j->HitPoints() <= 0)
					{
						PopupSprite(j->EnemyBox().x + 8, j->EnemyBox().y + 16, DEATH);
						j = enemies.erase(j);						
						score += 100;
						sm->play("break");						
					}
				}
				else
				{
					j++;
				}
			}

			if (i->Cooldown() == 60)
			{
				i = projectiles.erase(i);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
			}
			else
				i++;
		}

		//Check collision between projectiles and objects
		for (std::list<SubWeapon>::iterator i = projectiles.begin(); i != projectiles.end();)
		{
			for (std::list<DestroyableObject>::iterator j = objects.begin(); j != objects.end();)
			{
				if (checkCollision(i->SubWeaponBox(), j->ObjectBox()))
				{
					switch (j->Type())
					{
					case TORCH:						
						PopupSprite(j->ObjectBox().x, i->SubWeaponBox().y + 16, HIT);
						SpawnPickup(j->ObjectBox().x + 16, j->ObjectBox().y, 16);
						PopupSprite(j->ObjectBox().x + 8, j->ObjectBox().y + 16, DEATH);
						break;

					case CANDLE:						
						PopupSprite(j->ObjectBox().x, i->SubWeaponBox().y + 16, HIT);
						SpawnPickup(j->ObjectBox().x + 8, j->ObjectBox().y, 8);
						PopupSprite(j->ObjectBox().x + 4, j->ObjectBox().y + 8, DEATH);
						break;

					default:
						break;

					}		
					i->SetCooldown(60);	
					j = objects.erase(j);
					sm->play("break");
									
				}
				else
				{
					j++;
				}
			}

			if (i->Cooldown() == 60)
			{
				i = projectiles.erase(i);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
			}
			else
				i++;
		}

		//Turn on boss for boss fight
		if (player.BossFight())
			levelBoss.setAwake();

		//Boss physics and collisions
		if (levelBoss.Awake())
		{
			levelBoss.ApplyPhysics(player.PosX(), player.PosY(), levelTiles);

			SDL_Rect tempBox = levelBoss.BossBox();
			tempBox.x += 16;
			tempBox.w -= 32;

			if (checkCollision(player.PlayerBox(), tempBox))
			{
				if (player.HitCooldown() <= 0 && !player.PlayerDead())
				{
					if (player.PlayerBox().x < levelBoss.BossBox().x + 32)
						player.takeHitBoss(levelBoss.Type(), false);
					else
						player.takeHitBoss(levelBoss.Type(), true);

					sm->play("player_hurt");
				}
			}

			if (checkCollision(player.AttackBox(), levelBoss.BossBox()))
			{
				if (levelBoss.getCooldown() <= 0)
				{
					levelBoss.takeHit(player.WhipLevel());
					sm->play("break");

					if (!player.Flip())
						PopupSprite(player.AttackBox().x + 80, player.AttackBox().y - 4, HIT);
					else
						PopupSprite(player.AttackBox().x, player.AttackBox().y - 4, HIT);

					if (levelBoss.Health() <= 0)
					{
						PopupSprite(levelBoss.BossBox().x + 16, levelBoss.BossBox().y - 8, DEATH);
						PopupSprite(levelBoss.BossBox().x + 32, levelBoss.BossBox().y - 8, DEATH);
						PopupSprite(levelBoss.BossBox().x + 48, levelBoss.BossBox().y - 8, DEATH);
						PopupSprite(levelBoss.BossBox().x + 64, levelBoss.BossBox().y - 8, DEATH);

						PopupSprite(levelBoss.BossBox().x + 16, levelBoss.BossBox().y + 24, DEATH);
						PopupSprite(levelBoss.BossBox().x + 32, levelBoss.BossBox().y + 24, DEATH);
						PopupSprite(levelBoss.BossBox().x + 48, levelBoss.BossBox().y + 24, DEATH);
						PopupSprite(levelBoss.BossBox().x + 64, levelBoss.BossBox().y + 24, DEATH);

						Pickup newPickup;
						newPickup.setPickup(1264, 240, PickupType::SPIRIT_BALL);
						pickups.push_back(newPickup);


						levelBoss.setBoss(0, 0, BossType::FINISHED, totalTiles);
					}
				}
			}
		}
		if (player.Splash() && playerSplash)
		{			
			PopupSprite(player.PlayerBox().x - 32, player.PlayerBox().y + 32, SPLASH_LEFT);
			PopupSprite(player.PlayerBox().x + 32, player.PlayerBox().y + 32, SPLASH_RIGHT);
			PopupSprite(player.PlayerBox().x, player.PlayerBox().y - 32, SPLASH_RIGHT);
			sm->play("splash_down");
			playerSplash = false;
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
		it3->DrawPickup(tm, renderer, camera, frameCount);
	}

	std::list<SpritePopup>::iterator it4;
	for (it4 = popups.begin(); it4 != popups.end(); it4++)
	{
		it4->DrawPopup(tm, renderer, camera, frameCount);
	}

	std::list<SubWeapon>::iterator it5;
	for (it5 = projectiles.begin(); it5 != projectiles.end(); it5++)
	{
		it5->DrawSubWeapon(tm, renderer, camera, frameCount);
	}

	//Draw Boss
	levelBoss.DrawBoss(tm, renderer, camera, frameCount);

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
	if (levelName == "level1")
	{
		tm->draw(levelName, 0 - camera.x, 0 - camera.y, levelWidth + 512, levelHeight, renderer, SDL_FLIP_NONE);
		if (doorTimer < 384 && doorTimer >= 376 || doorTimer < 296 && doorTimer >= 288)
			tm->drawFrame("door", 3040 - camera.x, 112, 48, 96, 1, 1, renderer, SDL_FLIP_NONE);
		if (doorTimer < 376 && doorTimer >= 296)
			tm->drawFrame("door", 3056 - camera.x, 112, 48, 96, 1, 0, renderer, SDL_FLIP_NONE);
	}		

	if (levelName == "level2")
	{
		tm->draw(levelName, 0 - camera.x, 0 - camera.y, levelWidth + 512, levelHeight, renderer, SDL_FLIP_NONE);
		if (doorTimer < 384 && doorTimer >= 376 || doorTimer < 296 && doorTimer >= 288)
			tm->drawFrame("door", 992 - camera.x, 112, 48, 96, 1, 1, renderer, SDL_FLIP_NONE);
		if (doorTimer < 376 && doorTimer >= 296)
			tm->drawFrame("door", 1008 - camera.x, 112, 48, 96, 1, 0, renderer, SDL_FLIP_NONE);
	}

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
		//Boss levelBoss;

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
		case '6':
			//Water location
			levelTiles[i] = new Tile(x, y, '6');
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


////////OBJECTS//////////////////////////////////////////////////
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


////////ENEMIES//////////////////////////////////////////////////
		case 'g':	
			//Ghoul spawnpoint
			newEnemy.setEnemy(x, y - 32, EnemyType::GHOUL);
			enemies.push_back(newEnemy);
			levelTiles[i] = new Tile(x, y, '0');
			break;

		case 'p':
			//Panther spawnpoint
			newEnemy.setEnemy(x, y, EnemyType::PANTHER);
			enemies.push_back(newEnemy);
			levelTiles[i] = new Tile(x, y, '0');
			break;

////////SPAWNPOINTS AND EXITS////////////////////////////////////
		case 'P':
			//Player spawnpoint
			spawnPointX = x - 12;
			spawnPointY = y;
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case '8':
			//Player spawnpoint 2
			spawnPoint2X = x - 12;
			spawnPoint2Y = y;
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case '9':
			//Player spawnpoint 3
			spawnPoint3X = x - 12;
			spawnPoint3Y = y;
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'X':
			//Level exit
			levelTiles[i] = new Tile(x, y, 'X');
			break;

		case 'W':
			//Level exit 2
			levelTiles[i] = new Tile(x, y, 'W');
			break;

		case 'D':
			//Level door
			levelTiles[i] = new Tile(x, y, 'D');
			break;

		case 'M':
			//Slow walk to door right
			levelTiles[i] = new Tile(x, y, 'W');
			break;
		case 'N':
			//Slow walk to door left
			levelTiles[i] = new Tile(x, y, 'V');
			break;

		case 'Z':
			//Exit area backwards
			levelTiles[i] = new Tile(x, y, 'Z');
			break;

		case 'Y':
			//Exit area backwards
			levelTiles[i] = new Tile(x, y, 'Y');
			break;

////////BOSS FIGHT TILES/////////////////////////////////////////////
		case '4':
			//Boss fight engage
			levelTiles[i] = new Tile(x, y, 'B');
			break;

		case '3':
			//Boss fight wall tiles, to prevent leaving area
			levelTiles[i] = new Tile(x, y, '3');
			break;

		case 'B':
			//Boss location
			levelTiles[i] = new Tile(x, y, '0');
			levelBoss.setBoss(x - 18, y + 10, BossType::VAMPIRE_BAT, totalTiles);
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

void Level::SetScore(int value)
{
	score = value;
}

void Level::SpawnEnemies(SDL_Rect& camera)
{
	spawnCooldown--;
	if (levelName == "level1")
	{
		if (player.PosX() < 832 && player.PosX() > 160 && spawnCooldown <= 0 ||
			player.PosX() < 2496 && player.PosX() > 2112 && spawnCooldown <= 0)
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

	case 'D':
		newPickup.setPickup(x - offset, y, DAGGER_DROP);
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

void Level::SpawnProjectile(int x, int y, int directionValue, SubWeaponType type)
{
	SubWeapon newSubWeapon;

	switch (type)
	{
	case EMPTY:
		break;

	case DAGGER:
		newSubWeapon.setSubWeapon(x, y, directionValue, DAGGER);
		break;

	default:
		break;
	}
	projectiles.push_back(newSubWeapon);

	player.SetCurrentProjectiles(player.CurrentProjectiles() + 1);
	player.SetSpawnProjectile(false);	
}

bool Level::LevelComplete()
{
	return levelComplete;
}