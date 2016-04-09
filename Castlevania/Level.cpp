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
	levelObjects.clear();

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
	case 6:
		levelWidth = 1024;
		break;
	case 7:
		levelWidth = 1536;
		break;
	case 8:
		levelWidth = 1536;
		break;
	case 9:
		levelWidth = 1536;
		break;
	case 10:
		levelWidth = 1536;
		break;
	case 11:
		levelWidth = 1536;
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
		player.SetOnStairs(false, true);
		
	if (levelN == 2 && previousLevel || levelN == 2 && previousLevel2)
		player.SetOnStairs(true, false);

	if (levelN == 5 && previousLevel)
		player.SetOnStairs(false, false);

	if (levelN == 6 || levelN == 8 || levelN == 10)
		player.SetOnStairs(true, true);

	if (levelN == 7 && previousLevel)
		player.SetOnStairs(false, false);

	if (levelN == 7 && !previousLevel || levelN == 9 || levelN == 11)
		player.SetFlip(true);

	score = 0;	
	previousLevel = false;
	previousLevel2 = false;
	secondExit = false;

	levelComplete = false;

	playerSplash = true;

	pause = false;
	pauseTimer = 0;
	stopwatch = false;
	stopwatchTimer = 0;
}

void Level::UpdateLevel(SDL_Rect& camera, SoundManager* sm)
{
	if (pause)
		pauseTimer--;

	if (pauseTimer <= 0)
		pause = false;

	if (stopwatchTimer > 0)
		stopwatchTimer--;

	if (stopwatchTimer == 0)
		stopwatch = false;

	if (rosaryTimer > 0)
		rosaryTimer--;

	if (!pause)
	{
		if (player.SpawnProjectile())
		{
			if (player.SubWeapon() != SubWeaponType::STOPWATCH)
			{
				if (!player.Flip())
					SpawnProjectile(player.PosX(), player.PosY(), 1, player.SubWeapon());
				else
					SpawnProjectile(player.PosX(), player.PosY(), -1, player.SubWeapon());

				if (player.SubWeapon() == SubWeaponType::DAGGER)
					sm->play("player_throw");
			}
			else if (player.SubWeapon() == SubWeaponType::STOPWATCH && stopwatchTimer <= 0)
			{
				stopwatch = true;
				stopwatchTimer = 120;
				sm->play("stopwatch");
				player.SetSpawnProjectile(false);
			}
		}

		SpawnEnemies(camera, sm);

		//Iterate through enemies to apply their physics and check for collisions
		std::list<Enemy>::iterator it2 = enemies.begin();			
		while (it2 != enemies.end())
		{
			if (!stopwatch)
				it2->ApplyPhysics(levelTiles, totalTiles, player.PlayerBox());

			SDL_Rect playerBox = player.PlayerBox();

			//change player collision box if crouching or jumping
			if (player.Crouching())
			{
				playerBox.h = 48;
				playerBox.y += 16;
			}
			else if (player.Jumping())
			{
				playerBox.h = 48;
				playerBox.y -= 16;
			}

			//Iterate through enemies to see if they hit the player
			if (checkCollision(playerBox, it2->EnemyBox()))
			{	
				
				if (player.HitCooldown() <= 0 && !player.PlayerDead())
				{
					if (playerBox.x < it2->EnemyBox().x)
						player.takeHit(it2->Type(), false);
					else
						player.takeHit(it2->Type(), true);

					sm->play("player_hurt");

					if (it2->Type() == EnemyType::BAT)
					{
						PopupSprite(it2->EnemyBox().x + (it2->EnemyBox().w / 2) - 8,
							it2->EnemyBox().y + (it2->EnemyBox().h / 2) - 16, DEATH);
						it2 = enemies.erase(it2);
						score += 200;						
						break;
					}
					if (it2->Type() == EnemyType::PROJECTILE)
					{
						PopupSprite(it2->EnemyBox().x + (it2->EnemyBox().w / 2) - 8,
							it2->EnemyBox().y + (it2->EnemyBox().h / 2) - 16, DEATH);
						it2 = enemies.erase(it2);
						break;
					}
					if (it2->Type() == EnemyType::PROJECTILE_FAST)
					{
						PopupSprite(it2->EnemyBox().x + (it2->EnemyBox().w / 2) - 8,
							it2->EnemyBox().y + (it2->EnemyBox().h / 2) - 16, DEATH);
						it2 = enemies.erase(it2);
						break;
					}
				}
			}	

			/*if (it2->EnemyBox().x - playerBox.x < 128 && it2->Type() == EnemyType::PANTHER)
			{
				it2->SetIdle(false);
			}*/

			if (it2->Splash() && it2->Type() == EnemyType::FISHMAN)
			{
				PopupSprite(it2->EnemyBox().x - 32, it2->EnemyBox().y + 32, SPLASH_LEFT);
				PopupSprite(it2->EnemyBox().x + 32, it2->EnemyBox().y + 32, SPLASH_RIGHT);
				PopupSprite(it2->EnemyBox().x, it2->EnemyBox().y - 32, SPLASH_RIGHT);
				sm->play("splash_down");
				it2 = enemies.erase(it2);
				break;
			}

			//Certain enemies shoot projectiles
			if (it2->DoShot() && it2->Type() == EnemyType::FISHMAN)
			{
				Enemy newEnemyProjectile;
				if (it2->Flip())
					newEnemyProjectile.setEnemy(it2->EnemyBox().x + 16, it2->EnemyBox().y, EnemyType::PROJECTILE, true);
				else
					newEnemyProjectile.setEnemy(it2->EnemyBox().x + 16, it2->EnemyBox().y, EnemyType::PROJECTILE, false);
				enemies.push_back(newEnemyProjectile);				
			}

			if (it2->DoShot() && it2->Type() == EnemyType::BONE_TOWER)
			{
				Enemy newEnemyProjectile;
				if (playerBox.x < it2->EnemyBox().x)
					newEnemyProjectile.setEnemy(it2->EnemyBox().x + 16, it2->EnemyBox().y + 12, EnemyType::PROJECTILE_FAST, true);
				else
					newEnemyProjectile.setEnemy(it2->EnemyBox().x + 16, it2->EnemyBox().y + 12, EnemyType::PROJECTILE_FAST, false);
				enemies.push_back(newEnemyProjectile);
			}

			//Iterate through enemies to see if they need to be erased
			if (checkCollision(player.AttackBox(), it2->EnemyBox()))
			{
				if (it2->getCooldown() <= 0)
				{
					it2->takeHit(player.WhipLevel());
					PopupSprite(it2->EnemyBox().x, player.AttackBox().y - 4, HIT);
					if (it2->Type() == EnemyType::BONE_TOWER)
						sm->play("enemy_hit");
					else
						sm->play("break");
					if (it2->HitPoints() <= 0.0f)
					{
						PopupSprite(it2->EnemyBox().x + (it2->EnemyBox().w / 2) - 8,
							it2->EnemyBox().y + (it2->EnemyBox().h / 2) - 16, DEATH);

						switch (it2->Type())
						{
						case GHOUL:
							score += 100;
							break;

						case PANTHER:
							score += 200;
							break;

						case BAT:
							score += 200;
							break;

						case BAT_BLUE:
							score += 200;
							break;

						case FISHMAN:
							score += 300;
							break;

						case SPEAR_KNIGHT:
							score += 400;
							break;

						case MEDUSA_HEAD:
							score += 300;
							break;

						case BONE_TOWER:
							sm->play("break");
							score += 400;
							break;

						default:
							break;
						}

						it2 = enemies.erase(it2);
					}
				}
				else
					break;
			}
			else if (playerBox.x - 384 > it2->EnemyBox().x && !it2->Idle() || it2->EnemyBox().x < 0)
			{
				it2 = enemies.erase(it2);
			}
			else if (it2->DeleteEnemy())
				it2 = enemies.erase(it2);
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

				case TILE_1:
					sm->play("break_tile");
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y, DEBRIS_LEFT);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y + 16, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y + 16, DEBRIS_RIGHT);
					SpawnPickup(it->ObjectBox().x, it->ObjectBox().y, 0);					
					break;

				case TILE_2:
					sm->play("break_tile");
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y, DEBRIS_LEFT);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y + 16, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y + 16, DEBRIS_RIGHT);
					SpawnPickup(it->ObjectBox().x, it->ObjectBox().y, 0);
					break;

				case TILE_3:
					sm->play("break_tile");
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y, DEBRIS_LEFT);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y + 16, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y + 16, DEBRIS_RIGHT);
					SpawnPickup(it->ObjectBox().x, it->ObjectBox().y, 0);
					break;
				case TILE_4:
					sm->play("break_tile");
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y, DEBRIS_LEFT);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x, it->ObjectBox().y + 16, DEBRIS_UP);
					PopupSprite(it->ObjectBox().x + 16, it->ObjectBox().y + 16, DEBRIS_RIGHT);
					SpawnPickup(it->ObjectBox().x, it->ObjectBox().y, 0);
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
					if (player.WhipLevel() < 3)
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
					if (player.SubWeapon() != SubWeaponType::DAGGER)
					{
						player.SetSubWeapon(SubWeaponType::DAGGER);
						player.SetMaxProjectiles(1);
					}
					break;

				case STOPWATCH_DROP:
					sm->play("weapon");
					player.SetSubWeapon(SubWeaponType::STOPWATCH);
					player.SetMaxProjectiles(1);
					break;

				case HOLY_WATER_DROP:
					sm->play("weapon");
					if (player.SubWeapon() != SubWeaponType::HOLY_WATER)
					{
						player.SetSubWeapon(SubWeaponType::HOLY_WATER);
						player.SetMaxProjectiles(1);
					}
					break;

				case AXE_DROP:
					sm->play("weapon");
					if (player.SubWeapon() != SubWeaponType::AXE)
					{
						player.SetSubWeapon(SubWeaponType::AXE);
						player.SetMaxProjectiles(1);
					}
					break;

				case CROSS_DROP:
					sm->play("weapon");
					if (player.SubWeapon() != SubWeaponType::CROSS)
					{
						player.SetSubWeapon(SubWeaponType::CROSS);
						player.SetMaxProjectiles(1);
					}
					
					break;

				case ROSARY:
					rosaryTimer = 15;
					sm->play("rosary");
					enemies.clear();				
					break;

				case ROAST:
					sm->play("weapon");
					if (player.Health() >= 10)
						player.SetHealth(16);
					else
						player.SetHealth(player.Health() + 6);
					break;

				case DOUBLE_SHOT:
					sm->play("weapon");
					if (player.SubWeapon() != SubWeaponType::STOPWATCH)
					{
						player.SetMaxProjectiles(2);
					}
					else
						score += 700;
					
					break;

				case TRIPLE_SHOT:
					sm->play("weapon");
					if (player.SubWeapon() != SubWeaponType::STOPWATCH)
					{
						player.SetMaxProjectiles(3);
					}
					else
						score += 700;
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
			else if (it3->Type() != PickupType::SPIRIT_BALL && it3->PickupTimer() > 180)
			{
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
			it5->ApplyPhysics(levelTiles, totalTiles);

			if (it5->Type() == SubWeaponType::HOLY_WATER && it5->FlameTimer() == 1)
				sm->play("holy_water");

			if (it5->Type() == SubWeaponType::AXE && it5->SFXTimer() == 15)
				sm->play("player_whip");

			if (it5->Type() == SubWeaponType::CROSS && it5->SFXTimer() == 10)
				sm->play("cross");

			
			if (it5->Type() == SubWeaponType::AXE && it5->SubWeaponBox().y > 448)
			{
				it5 = projectiles.erase(it5);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
			}
			else if (it5->Type() == SubWeaponType::DAGGER && it5->CrossTimer() > 60)
			{
				it5 = projectiles.erase(it5);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
			}
			
			else if (it5->Type() == SubWeaponType::HOLY_WATER && it5->FlameTimer() >= 60)
			{
				it5 = projectiles.erase(it5);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
			}
			else if (it5->Type() == SubWeaponType::CROSS && checkCollision(player.PlayerBox(), it5->SubWeaponBox()) && it5->CrossTimer() > 75)
			{
				it5 = projectiles.erase(it5);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
			}
			else if (it5->Type() == SubWeaponType::CROSS && it5->CrossTimer() > 240)
			{				
				it5 = projectiles.erase(it5);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);				
			}
			else 
				it5++;
		}

		std::list<LevelObject>::iterator it6 = levelObjects.begin();
		while (it6 != levelObjects.end())
		{
			it6->ApplyPhysics(levelTiles, totalTiles); 

			if (it6->Type() == LevelObjectType::CRUSHER_1)
			{
				if (checkCollision(player.PlayerBox(), it6->LevelObjectBox()))
				{
					player.SetHealth(0);
				}
			}
			if (it6->Type() == LevelObjectType::CRUSHER_2 && player.Crouching() == false)
			{
				if (checkCollision(player.PlayerBox(), it6->LevelObjectBox()))
				{
					player.SetHealth(0);
				}
			}

			it6++;
		}

		//Check collision between projectiles and enemies
		for (std::list<SubWeapon>::iterator i = projectiles.begin(); i != projectiles.end();)
		{
			for (std::list<Enemy>::iterator j = enemies.begin(); j != enemies.end();) 
			{
				if (checkCollision(i->SubWeaponBox(), j->EnemyBox()) && j->getCooldown() <= 0)
				{
					
					j->takeHit(i->DamageValue());
					PopupSprite(j->EnemyBox().x, i->SubWeaponBox().y + 16, HIT);
					i->SetCooldown(60);
					if (j->HitPoints() <= 0.0f)
					{
						PopupSprite(j->EnemyBox().x + 8, j->EnemyBox().y + 16, DEATH);
						switch (j->Type())
						{
						case GHOUL:
							score += 100;
							break;

						case PANTHER:
							score += 200;
							break;

						case BAT:
							score += 200;
							break;

						case BAT_BLUE:
							score += 200;
							break;

						case FISHMAN:
							score += 300;
							break;

						case SPEAR_KNIGHT:
							score += 400;
							break;

						case MEDUSA_HEAD:
							score += 300;
							break;

						case BONE_TOWER:
							score += 400;
							break;

						default:
							break;
						}
						j = enemies.erase(j);						
						sm->play("break");

					}
					
				}
				else
				{
					j++;
				}
			}

			if (i->Cooldown() == 60 && i->Type() == SubWeaponType::DAGGER )
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

			if (i->Cooldown() == 60 && i->Type() == SubWeaponType::DAGGER)
			{
				i = projectiles.erase(i);
				player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
			}
			else
				i++;
		}

		//Turn on boss for boss fight
		if (player.BossFight())
		{			
			levelBoss.setAwake();			
		}
			

		//Boss physics and collisions
		if (levelBoss.Awake())
		{
			if (!stopwatch)
				levelBoss.ApplyPhysics(player.PosX(), player.PosY(), levelTiles);

			SDL_Rect tempBox = levelBoss.BossBox();
			tempBox.x += 16;
			tempBox.w -= 32;

			if (levelBoss.SpawnMob())
			{
				Enemy newEnemy;
				if (player.PlayerBox().x < levelBoss.BossBox().x)
					newEnemy.setEnemy(levelBoss.BossBox().x + 16, 316, EnemyType::SNAKE, true);
				else
					newEnemy.setEnemy(levelBoss.BossBox().x + 16, 316, EnemyType::SNAKE, false);
				enemies.push_back(newEnemy);
			}

			if (checkCollision(player.PlayerBox(), tempBox) && levelBoss.Awake())
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
						if (levelBoss.Type() == BossType::VAMPIRE_BAT)
						{
							newPickup.setPickup(1264, 240, PickupType::SPIRIT_BALL);
							score += 3000;
						}
						else if (levelBoss.Type() == BossType::MEDUSA)
						{						
							newPickup.setPickup(208, 176, PickupType::SPIRIT_BALL);
							score += 3000;
						}
						pickups.push_back(newPickup);

						
						levelBoss.setBoss(0, 0, BossType::FINISHED, totalTiles);
					}
				}
			}

			for (std::list<SubWeapon>::iterator k = projectiles.begin(); k != projectiles.end();)
			{
				if (checkCollision(k->SubWeaponBox(), levelBoss.BossBox()))
				{
					if (levelBoss.getCooldown() <= 0)
					{
						levelBoss.takeHit(k->DamageValue());
						sm->play("break");
						PopupSprite(k->SubWeaponBox().x + 16, k->SubWeaponBox().y + 16, HIT);

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
							if (levelBoss.Type() == BossType::VAMPIRE_BAT)
							{
								newPickup.setPickup(1264, 240, PickupType::SPIRIT_BALL);
								score += 3000;
							}
							else if (levelBoss.Type() == BossType::MEDUSA)
							{
								newPickup.setPickup(208, 176, PickupType::SPIRIT_BALL);
								score += 3000;
							}
							pickups.push_back(newPickup);


							levelBoss.setBoss(0, 0, BossType::FINISHED, totalTiles);
						}
					}
					if (k->Cooldown() == 60 && k->Type() == SubWeaponType::DAGGER)
					{
						k = projectiles.erase(k);
						player.SetCurrentProjectiles(player.CurrentProjectiles() - 1);
					}		
					else
					{
						k++;
						break;
					}
				}				
				k++;
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
			player.ApplyPhysics(levelTiles, levelObjects, objects);
			player.PlaySounds(sm);
			player.setCamera(camera);
		}

		
	}	
}

void Level::DrawLevel(TextureManager* tm, SDL_Renderer* renderer, SDL_Rect& camera, int frameCount)
{
	//Draw Level background	

	if (levelName == "level6" || levelName == "level8")
		tm->draw(levelName, 0 - camera.x - 512, 0 - camera.y, levelWidth + 512, levelHeight, renderer, SDL_FLIP_NONE);
	else
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

	std::list<SubWeapon>::iterator it5;
	for (it5 = projectiles.begin(); it5 != projectiles.end(); it5++)
	{
		it5->DrawSubWeapon(tm, renderer, camera, frameCount);
	}

	//Draw Boss
	levelBoss.DrawBoss(tm, renderer, camera, frameCount);

	//Draw Popups
	std::list<SpritePopup>::iterator it4;
	for (it4 = popups.begin(); it4 != popups.end(); it4++)
	{
		it4->DrawPopup(tm, renderer, camera, frameCount);
	}

	//Draw Level Objects
	std::list<LevelObject>::iterator it6;
	for (it6 = levelObjects.begin(); it6 != levelObjects.end(); it6++)
	{
		it6->DrawLevelObject(tm, renderer, camera);
	}

	if (levelName == "level9")
	{
		tm->draw("crushercover", 704 - camera.x, 80 - camera.y, 320, 112, renderer, SDL_FLIP_NONE);
	}

	//Draw Player, switch for various animations
	player.Draw(tm, renderer, camera, frameCount);

	//Piece of wall to cover player on first level
	if (levelName == "level0")
	{
		tm->draw("wall1", 1408 - camera.x, 80 - camera.y, 128, 288, renderer, SDL_FLIP_NONE);
	}	

	if (rosaryTimer > 0)
		tm->drawFrame("flash", 0, 0, 512, 448, 1, rosaryTimer % 2, renderer, SDL_FLIP_NONE);
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

	if (levelName == "level6")
	{
		tm->draw(levelName, 0 - camera.x - 512, 0 - camera.y, levelWidth + 512, levelHeight, renderer, SDL_FLIP_NONE);
		if (doorTimer < 384 && doorTimer >= 376 || doorTimer < 296 && doorTimer >= 288)
			tm->drawFrame("door", 0 - camera.x, 112, 48, 96, 1, 1, renderer, SDL_FLIP_HORIZONTAL);
		if (doorTimer < 376 && doorTimer >= 296)
			tm->drawFrame("door", -16 - camera.x, 112, 48, 96, 1, 0, renderer, SDL_FLIP_HORIZONTAL);
	}

	if (levelName == "level8")
	{
		tm->draw(levelName, 0 - camera.x - 512, 0 - camera.y, levelWidth + 512, levelHeight, renderer, SDL_FLIP_NONE);
		if (doorTimer < 384 && doorTimer >= 376 || doorTimer < 296 && doorTimer >= 288)
			tm->drawFrame("door", 0 - camera.x, 176, 48, 96, 1, 1, renderer, SDL_FLIP_HORIZONTAL);
		if (doorTimer < 376 && doorTimer >= 296)
			tm->drawFrame("door", -16 - camera.x, 176, 48, 96, 1, 0, renderer, SDL_FLIP_HORIZONTAL);
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
		LevelObject newLevelObject;

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
		case '5':
			//thin wall
			levelTiles[i] = new Tile(x + 16, y, '1');
			break;
		case '6':
			//Water location
			levelTiles[i] = new Tile(x, y, tileType);
			break;
		case '7':
			//Pitfall 
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
		case 'E':
			//Wall for enemies
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
		case 'c':
			//Candle objects to go on other tiles
			newObject.setObject(x - 8, y - 32, ObjectType::CANDLE);
			objects.push_back(newObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'I':
			//Tile 1 object
			newObject.setObject(x, y, ObjectType::TILE_1);
			objects.push_back(newObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'J':
			//Tile 2 object
			newObject.setObject(x, y, ObjectType::TILE_2);
			objects.push_back(newObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'K':
			//Tile 3 object
			newObject.setObject(x, y, ObjectType::TILE_3);
			objects.push_back(newObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'k':
			//Tile 4 object
			newObject.setObject(x, y, ObjectType::TILE_4);
			objects.push_back(newObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'A':
			//Platform
			newLevelObject.setLevelObject(x, y , LevelObjectType::PLATFORM);
			levelObjects.push_back(newLevelObject);
			levelTiles[i] = new Tile(x, y, '0');			
			break;
		case 'F':
			//Crusher
			newLevelObject.setLevelObject(x, y + 12, LevelObjectType::CRUSHER_1);
			levelObjects.push_back(newLevelObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		case 'f':
			//Crusher2
			newLevelObject.setLevelObject(x, y + 28, LevelObjectType::CRUSHER_2);
			levelObjects.push_back(newLevelObject);
			levelTiles[i] = new Tile(x, y, '0');
			break;
		

////////ENEMIES//////////////////////////////////////////////////
		case 'g':	
			//Ghoul spawnpoint
			newEnemy.setEnemy(x, y - 32, EnemyType::GHOUL, true);
			enemies.push_back(newEnemy);
			levelTiles[i] = new Tile(x, y, '0');
			break;

		case 'p':
			//Panther spawnpoint
			newEnemy.setEnemy(x, y, EnemyType::PANTHER, true);
			enemies.push_back(newEnemy);
			levelTiles[i] = new Tile(x, y, '0');
			break;

		case 's':
			//Spear Knight spawnpoint
			newEnemy.setEnemy(x, y, EnemyType::SPEAR_KNIGHT, true);
			enemies.push_back(newEnemy);
			levelTiles[i] = new Tile(x, y, '0');
			break;

		case 'b':
			//Blue Bat spawnpoint
			newEnemy.setEnemy(x + 16, y + 16, EnemyType::BAT_BLUE, false);			
			enemies.push_back(newEnemy);
			levelTiles[i] = new Tile(x, y, '0');
			break;

		case 't':
			//Bone tower spawnpoint
			newEnemy.setEnemy(x, y, EnemyType::BONE_TOWER, false);
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
			//Level door right
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

		case 'v':
			//Vampire Bat Boss location
			levelTiles[i] = new Tile(x, y, '0');			
			levelBoss.setBoss(x - 18, y + 10, BossType::VAMPIRE_BAT, totalTiles);
			break;

		case 'm':
			//Medusa Boss location
			levelTiles[i] = new Tile(x, y, '0');
			levelBoss.setBoss(x, y, BossType::MEDUSA, totalTiles);
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

	level.append(levelName.append("pickup.map"));

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

void Level::SpawnEnemies(SDL_Rect& camera, SoundManager* sm)
{
	if (!stopwatch && !pause)
		spawnCooldown--;

	if (levelName == "level1")
	{
		if (player.PosX() < 832 && player.PosX() > 160 || player.PosX() < 2528 && player.PosX() > 2112)
		{
			Enemy e1;
			if (spawnCooldown == 80)
			{
				if (player.PosX() % 32 < 16)
					e1.setEnemy(camera.x + 512, 336, EnemyType::GHOUL, true);
				else
					e1.setEnemy(camera.x + 32, 336, EnemyType::GHOUL, false);
			}
			if (spawnCooldown == 40)
			{
				if (player.PosX() % 32 < 16)
					e1.setEnemy(camera.x + 544, 336, EnemyType::GHOUL, true);
				else
					e1.setEnemy(camera.x + 16, 336, EnemyType::GHOUL, false);
			}
			if (spawnCooldown <= 0)
			{
				if (player.PosX() % 32 < 16)
					e1.setEnemy(camera.x + 576, 336, EnemyType::GHOUL, true);
				else
					e1.setEnemy(camera.x, 336, EnemyType::GHOUL, false);

				spawnCooldown = 420;
			}
			enemies.push_back(e1);
		}

		if (player.PosX() < 2976 && player.PosX() > 2528 )
		{
			Enemy e1;
			if (spawnCooldown == 80)
			{
				if (player.PosX() % 32 < 16)
					e1.setEnemy(2976, 336, EnemyType::GHOUL, true);
				else
					e1.setEnemy(camera.x + 32, 336, EnemyType::GHOUL, false);
			}
			if (spawnCooldown == 40)
			{
				if (player.PosX() % 32 < 16)
					e1.setEnemy(2976, 336, EnemyType::GHOUL, true);
				else
					e1.setEnemy(camera.x + 16, 336, EnemyType::GHOUL, false);
			}
			if (spawnCooldown <= 0)
			{
				if (player.PosX() % 32 < 16)
					e1.setEnemy(2976, 336, EnemyType::GHOUL, true);
				else
					e1.setEnemy(camera.x, 336, EnemyType::GHOUL, false);

				spawnCooldown = 420;
			}
			enemies.push_back(e1);
		}
	}

	if (levelName == "level2")
	{
		if (spawnCooldown <= 0)
		{
			Enemy e1;
			if (player.PosX() <= 256)
			{
				e1.setEnemy(player.PosX() + 384, player.PosY(), EnemyType::BAT, true);
				e1.SetIdle(false);
				enemies.push_back(e1);
			}
			else if (player.PosX() > 256 && player.PosX() <= 576)
			{
				e1.setEnemy(player.PosX() - 256, player.PosY(), EnemyType::BAT, false);
				e1.SetIdle(false);
				enemies.push_back(e1);
			}
			else if (player.PosX() > 576 && player.PosX() <= 800)
			{
				e1.setEnemy(player.PosX() + 256, player.PosY(), EnemyType::BAT, true);
				e1.SetIdle(false);
				enemies.push_back(e1);
			}
			else if (player.PosX() > 800)
			{
				e1.setEnemy(player.PosX() - 256, player.PosY(), EnemyType::BAT, false);
				e1.SetIdle(false);
				enemies.push_back(e1);
			}

			spawnCooldown = 240;
		}
	}

	if (levelName == "level3")
	{
		if (player.PosX() < 896)
		{
			Enemy e1;

			if (spawnCooldown <= 0)
				spawnCooldown = 480;			

			if (spawnCooldown == 360 && player.PosX() < 704)
			{
				e1.setEnemy(player.PosX() + 256, 288, EnemyType::FISHMAN, true);
				PopupSprite(e1.EnemyBox().x - 32, e1.EnemyBox().y + 32, SPLASH_LEFT);
				PopupSprite(e1.EnemyBox().x + 32, e1.EnemyBox().y + 32, SPLASH_RIGHT);
				PopupSprite(e1.EnemyBox().x, e1.EnemyBox().y - 32, SPLASH_RIGHT);
				enemies.push_back(e1);
				sm->play("splash_up");
			}
			if (spawnCooldown == 240)
			{
				e1.setEnemy(player.PosX() - 256, 288, EnemyType::FISHMAN, false);
				PopupSprite(e1.EnemyBox().x - 32, e1.EnemyBox().y + 32, SPLASH_LEFT);
				PopupSprite(e1.EnemyBox().x + 32, e1.EnemyBox().y + 32, SPLASH_RIGHT);
				PopupSprite(e1.EnemyBox().x, e1.EnemyBox().y - 32, SPLASH_RIGHT);
				enemies.push_back(e1);
				sm->play("splash_up");
			}
			if (spawnCooldown == 120)
			{
				e1.setEnemy(player.PosX() - 128, 288, EnemyType::FISHMAN, false);
				PopupSprite(e1.EnemyBox().x - 32, e1.EnemyBox().y + 32, SPLASH_LEFT);
				PopupSprite(e1.EnemyBox().x + 32, e1.EnemyBox().y + 32, SPLASH_RIGHT);
				PopupSprite(e1.EnemyBox().x, e1.EnemyBox().y - 32, SPLASH_RIGHT);
				enemies.push_back(e1);
				sm->play("splash_up");
			}
		}
	}
	if (levelName == "level4")
	{
		if (player.PosX() < 832 && player.PosX() > 192 && spawnCooldown <= 0)
		{
			Enemy e1, e2, e3;
			e1.setEnemy(player.PosX() + 320, 160, EnemyType::GHOUL, true);
			enemies.push_back(e1);
			e2.setEnemy(player.PosX() + 384, 160, EnemyType::GHOUL, true);
			enemies.push_back(e2);
			e3.setEnemy(player.PosX() + 320, 288, EnemyType::GHOUL, true);
			enemies.push_back(e3);

			spawnCooldown = 360;

		}
	}

	if (levelName == "level7")
	{
		if (player.PosX() > 960 && spawnCooldown <= 0)
		{
			Enemy e1;

			if (player.Flip())
			{
				e1.setEnemy(camera.x, player.PosY() + 16, EnemyType::MEDUSA_HEAD, false);
				enemies.push_back(e1);				
			}
			else if (!player.Flip())
			{
				e1.setEnemy(camera.x + 512, player.PosY() + 16, EnemyType::MEDUSA_HEAD, true);
				enemies.push_back(e1);				
			}


			spawnCooldown = 150;
		}
	}

	if (levelName == "level8")
	{
		if (player.PosX() < 544 && spawnCooldown <= 0)
		{
			Enemy e1;

			if (player.Flip())
			{
				e1.setEnemy(camera.x, player.PosY() + 16, EnemyType::MEDUSA_HEAD, false);
				enemies.push_back(e1);
			}
			else if (!player.Flip())
			{
				e1.setEnemy(camera.x + 512, player.PosY() + 16, EnemyType::MEDUSA_HEAD, true);
				enemies.push_back(e1);
			}


			spawnCooldown = 150;
		}
	}

	if (levelName == "level10")
	{
		if (player.PosX() > 704 && player.PosX() < 1184 && spawnCooldown <= 0)
		{
			Enemy e1;

			if (player.Flip())
			{
				e1.setEnemy(camera.x, player.PosY() + 16, EnemyType::MEDUSA_HEAD, false);
				enemies.push_back(e1);
			}
			else if (!player.Flip())
			{
				e1.setEnemy(camera.x + 512, player.PosY() + 16, EnemyType::MEDUSA_HEAD, true);
				enemies.push_back(e1);
			}


			spawnCooldown = 150;
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
		if (player.WhipLevel() < 3)
			newPickup.setPickup(x - offset, y, WHIP);
		else
			newPickup.setPickup(x - offset, y, SMALL_HEART);
		break;

	case '2':
		if (player.WhipLevel() < 3)
			newPickup.setPickup(x - offset, y, WHIP);
		else
			newPickup.setPickup(x - offset, y, RED_MONEYBAG);
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

	case 'A':
		newPickup.setPickup(x - offset, y, AXE_DROP);
		break;

	case 'H':
		newPickup.setPickup(x - offset, y, HOLY_WATER_DROP);
		break;

	case 'T':
		newPickup.setPickup(x - offset, y, STOPWATCH_DROP);
		break;

	case 'X':
		newPickup.setPickup(x - offset, y, CROSS_DROP);
		break;

	case 'Y':
		newPickup.setPickup(x - offset, y, ROSARY);
		break;

	case 'Z':
		newPickup.setPickup(x - offset, y, ROAST);
		break;

	case '8':
		if (player.SubWeapon() == SubWeaponType::EMPTY)
			newPickup.setPickup(x - offset, y, RED_MONEYBAG);
		else if (player.MaxProjectiles() == 1)
			newPickup.setPickup(x - offset, y, DOUBLE_SHOT);
		else if (player.MaxProjectiles() >= 2)
			newPickup.setPickup(x - offset, y, TRIPLE_SHOT);
		break;
		
	case '9':
		//newPickup.setPickup(x - offset, y, TRIPLE_SHOT);
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

	case HOLY_WATER:
		newSubWeapon.setSubWeapon(x, y, directionValue, HOLY_WATER);
		break;

	case AXE:
		newSubWeapon.setSubWeapon(x, y, directionValue, AXE);
		break;

	case CROSS:
		newSubWeapon.setSubWeapon(x, y, directionValue, CROSS);
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

bool Level::Stopwatch()
{
	return stopwatch;
}

bool Level::Pause()
{
	return pause;
}