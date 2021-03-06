#include "App.h"
#include "Enemy.h"
#include "Player.h"
#include "GUI.H"
#include "EntityManager.h"
#include "SceneManager.h"
#include "DialogSystem.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

Enemy::Enemy() : Entity()
{
	name.Create("Enemy");
}

Enemy::Enemy(Entity* entity, SDL_Texture* tex)
{
	entityData = entity->entityData;
	entityData.texture = tex;
	stats = entity->stats;
	move = entity->move;

	name.Create("Enemy");
}

Enemy::~Enemy()
{}

bool Enemy::Start()
{
	active = true;

	if (entityData.id != 0)
	{
		switch (entityData.type)
		{
		case BANDIT:
			if (!move) entityData.currentAnimation = app->entityManager->animations.At(0)->data;
			else entityData.currentAnimation = app->entityManager->animations.At(1)->data;
			break;
		case FIGHTER:
			if (!move) entityData.currentAnimation = app->entityManager->animations.At(2)->data;
			else entityData.currentAnimation = app->entityManager->animations.At(3)->data;
			break;
		case SAPLING:
			entityData.currentAnimation = app->entityManager->animations.At(4)->data;
			break;
		default:
			break;
		}
	}

	// Enemy Path
	destination = entityData.positionInitial;

	radiusCollision = app->entity->CalculateDistance(entityData.pointsCollision[0], entityData.pointsCollision[2]) / 2 - 15;

	return true;
}

bool Enemy::Awake(pugi::xml_node& config)
{
	LOG("Loading Enemy Parser");
	bool ret = true;
	
	return ret;
}

void Enemy::CreatePathEnemy(iPoint mapPositionEnemy, iPoint mapPositionDestination)
{
	// Create the path for enemies
}

int Enemy::GetCurrentPositionInPath(iPoint mapPositionEnemy)
{
	int i;
	for (i = 0; i < lastPath->Count(); i++)
	{
		if (mapPositionEnemy == iPoint({ lastPath->At(i)->x, lastPath->At(i)->y })) break;
	}
	return i;
}

void Enemy::CheckCollisionEnemyToPlayer()
{
	iPoint enemyCenter;
	enemyCenter.x = entityData.position.x + entityData.pointsCollision[0].x + entityData.centerPoint.x;
	enemyCenter.y = entityData.position.y + entityData.pointsCollision[0].y + entityData.centerPoint.y;

	iPoint playerCenter;
	playerCenter.x = app->player->playerData.position.x + app->player->playerData.pointsCollision[0].x + app->player->playerData.centerPoint.x;
	playerCenter.y = app->player->playerData.position.y + app->player->playerData.pointsCollision[0].y + app->player->playerData.centerPoint.y;
	
	if (radiusCollision + app->player->radiusCollision > app->entity->CalculateDistance(playerCenter, enemyCenter) 
		&& !app->sceneManager->GetEnemeyDetected())
	{
		app->sceneManager->SetEnemeyDetected(true);
		entityData.state = DEAD;
		app->SaveGameRequest();
		app->entityManager->SetCurrentEntity(this);
		LOG("Collision Detected");
	}
		
}
bool Enemy::CheckCollisionEnemy(fPoint nextPosition)
{
	iPoint positionMapEnemy;
	int y = nextPosition.y;
	int x = nextPosition.x;

	for (int i = 0; i < entityData.numPoints; i++)
	{
		// Convert position player WorldToMap 
		positionMapEnemy = app->map->WorldToMap(x + entityData.pointsCollision[i].x, y + entityData.pointsCollision[i].y);
		if (CheckCollision(positionMapEnemy) == COLLISION) return true;
	}
	return false;
}
bool Enemy::Radar(iPoint distance, int range)
{
	fPoint distanceToPlayer;
	distanceToPlayer.x = distance.x;
	distanceToPlayer.y = distance.y;
	if (entityData.position.DistanceManhattan(distanceToPlayer) < range) return true;

	return false;
}
void Enemy::MoveEnemy()
{
	tmp = entityData.position;
	iPoint enemyCenter;
	enemyCenter.x = entityData.position.x + entityData.pointsCollision[0].x + entityData.centerPoint.x;
	enemyCenter.y = entityData.position.y + entityData.pointsCollision[0].y + entityData.centerPoint.y;

	iPoint playerCenter;
	playerCenter.x = app->player->playerData.position.x + app->player->playerData.pointsCollision[0].x + app->player->playerData.centerPoint.x;
	playerCenter.y = app->player->playerData.position.y + app->player->playerData.pointsCollision[0].y + app->player->playerData.centerPoint.y;

	if (returning)
	{
		float angle = atan2(entityData.positionInitial.y - entityData.position.y, entityData.positionInitial.x - entityData.position.x);
		float direction = entityData.velocity * cos(angle);

		entityData.position.x += entityData.velocity * cos(angle);
		entityData.position.y += entityData.velocity * sin(angle);

		if (direction < 0) entityData.direction = WALK_L;
		else entityData.direction = WALK_R;

		iPoint enemyPosition;
		enemyPosition.x = entityData.position.x;
		enemyPosition.y = entityData.position.y;

		if (app->map->WorldToMap(enemyPosition).x == app->map->WorldToMap(entityData.positionInitial).x)
			returning = false;
	}
	else if (isDetected)
	{
		float angle = atan2(playerCenter.y - enemyCenter.y, playerCenter.x - enemyCenter.x);
		float direction = entityData.velocity * cos(angle);

		entityData.position.x += entityData.velocity * cos(angle);
		entityData.position.y += entityData.velocity * sin(angle);

		if (direction < 0) entityData.direction = WALK_L;
		else entityData.direction = WALK_R;
	}
	else
	{
		if (entityData.id == 1 || entityData.id == 5 || entityData.id >= 7)
		{
			int numTiles = 0;
			if (entityData.id == 1 || (entityData.id >=9 && entityData.id <= 11))
				numTiles = 5;
			else if (entityData.id == 5 || (entityData.id >= 18 && entityData.id <= 20))
				numTiles = 6;
			else if (entityData.id == 7 || (entityData.id >= 12 && entityData.id <= 14))
				numTiles = 7;
			else if (entityData.id == 8 || entityData.id == 21)
				numTiles = 3;

			if (enemyCenter.x < destination.x)
			{
				entityData.position.x += entityData.velocity;
				entityData.direction = WALK_R;
			}
			else if (enemyCenter.x > destination.x)
			{
				entityData.position.x -= entityData.velocity;
				entityData.direction = WALK_L;
			}
			
			if (app->map->WorldToMap(enemyCenter).x == app->map->WorldToMap(destination).x)
			{
				if (destination.x == entityData.positionInitial.x)
					destination.x = entityData.positionInitial.x + numTiles * app->map->data.tileWidth;
				else destination.x = entityData.positionInitial.x;
			}
		}
	}
	
	if (CheckCollisionEnemy(entityData.position)) entityData.position = tmp;
	
}
bool Enemy::PreUpdate()
{
	return true;
}

bool Enemy::Update(float dt)
{
	if (entityData.id != 0 && app->sceneManager->GetCurrentScene()->type != SceneType::BATTLE)
	{
		entityData.velocity = floor(1000 * dt) / 16;

		if(entityData.type != NPC)
		{
			if (Radar(app->player->playerData.position, range))
			{
				if(isDetected == false) app->audio->PlayFx(app->entityManager->fxEnemyFound);
				isDetected = true;
				app->guiManager->enemyCloud->Update();
			}
			else isDetected = false;

			if (!Radar(entityData.positionInitial, rangeMax)) returning = true;
			else if (isDetected == true && app->player->playerData.state != IDLE) returning = false;
		}
		else
		{
			//if (Radar(app->player->playerData.position, 75) && (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN || (app->input->pads[0].a == true && !app->dialogueSystem->missClick)))
			if (Radar(app->player->playerData.position, 50)) app->guiManager->talkCloud->Update();

			if (!app->dialogueSystem->pendingDialog && Radar(app->player->playerData.position, 50) && (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN || (app->input->pads[0].a == true && !app->dialogueSystem->missClick)))
			{
				app->dialogueSystem->currentNode = app->dialogueSystem->dialogueTrees[entityData.id]->dialogueNodes[0];
				app->dialogueSystem->PerformDialogue(entityData.id, 7);
				app->dialogueSystem->SetId(entityData.id);
				
				app->dialogueSystem->pendingDialog = true;
				app->dialogueSystem->spawnDialog = true;
				app->dialogueSystem->despawnDialog = true;
				app->dialogueSystem->missClick = true;
				app->sceneManager->onDialog = true;

				if (entityData.id == 13 || entityData.id == 14)
				{
					app->audio->PlayFx(app->entityManager->fxCoffeButtons);
				}
			}
		}

		if(move)MoveEnemy();

		if (isDetected) CheckCollisionEnemyToPlayer();
	}

	if (entityData.type != NPC ) entityData.currentAnimation->Update();
	//&& entityData.id != 0
	return true;
}

bool Enemy::PostUpdate()
{
	if (entityData.type != NPC)
	{

		SDL_Rect rectEnemy;
		rectEnemy = entityData.currentAnimation->GetCurrentFrame();

		// Draw player in correct direction
		if (entityData.direction == MoveDirection::WALK_R)
			app->render->DrawTexture(entityData.texture, entityData.position.x, entityData.position.y, &rectEnemy);
		else if (entityData.direction == MoveDirection::WALK_L)
			app->render->DrawTextureFlip(entityData.texture, entityData.position.x, entityData.position.y, &rectEnemy);

		if (app->map->GetDraw())
		{
			iPoint enemyCenter;
			enemyCenter.x = entityData.position.x + entityData.pointsCollision[0].x + entityData.centerPoint.x;
			enemyCenter.y = entityData.position.y + entityData.pointsCollision[0].y + entityData.centerPoint.y;

			iPoint playerCenter;
			playerCenter.x = app->player->playerData.position.x + app->player->playerData.pointsCollision[0].x + app->player->playerData.centerPoint.x;
			playerCenter.y = app->player->playerData.position.y + app->player->playerData.pointsCollision[0].y + app->player->playerData.centerPoint.y;

			app->render->DrawLine(enemyCenter.x, enemyCenter.y, playerCenter.x, playerCenter.y, 100, 100, 100);
		}

		if (isDetected)
		{
			app->render->DrawTexture(app->guiManager->iconsUiTex, entityData.position.x + 20, entityData.position.y - 50, &app->guiManager->enemyCloud->GetCurrentFrame());
		}
	}
	else
	{
		if (Radar(app->player->playerData.position, 50)) app->entityManager->drawCloud = true;
	}
	return true;
}

bool Enemy::CleanUp()
{

	if (!active)
		return true;

	delete[] entityData.pointsCollision;
	entityData.pointsCollision = nullptr;

	delete lastPath;
	lastPath = nullptr;

	pendingToDelete = true;
	active = false;

	return true;
}
