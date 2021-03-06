#include "EntityManager.h"
#include "SceneManager.h"
#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entity_manager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before EntityManager is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	bool ret = true;


	return ret;
}

// Called before the first frame
bool EntityManager::Start()
{
	LOG("EntityManager start");
	active = true;


	// Load Textures
	texEnemies = app->tex->Load("Textures/Enemies/enemies_map.png");
	texObstacles = app->tex->Load("Textures/Objects/objects.png");

	// Load Fx
	fxCoffeButtons = app->audio->LoadFx("Audio/Fx/coffe_buttons.wav");
	fxEnemyFound = app->audio->LoadFx("Audio/Fx/enemy_triggers.wav");

	// Animations
	int numEnemies = 3;
	int numSprites = 1;
	for (int i = 0; i < numEnemies * 2; i++)
	{
		Animation* b = new Animation;
		if (i % 2) numSprites = 6;
		else numSprites = 4;
		for (int j = 0; j < numSprites; j++)
		{
			b->PushBack({ 64 * j, 32 * i, 64, 32 });
		}
		animations.Add(b);
	}

	return true;
}

// Called each loop iteration
bool EntityManager::PreUpdate()
{
	for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next)
		entiti->data->PreUpdate();

	return true;
}

bool EntityManager::Update(float dt)
{
	if (!app->sceneManager->GetIsPause())
	{
		CheckDespawnEntities();
		SpeedAnimationCheck(dt);
		CheckSpawnEntities();

		for (ListItem<Entity*>* entity = entities.start; entity; entity = entity->next) 
			entity->data->Update(dt);
		for (ListItem<Entity*>* partner = partners.start; partner; partner = partner->next)
			partner->data->Update(dt);
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	if ((!app->sceneManager->GetWinBattle() && app->sceneManager->GetTransitionToBattleFinish() && partners.start != NULL) || partners.start == NULL)
	{
		for (ListItem<Entity*>* entity = entities.start; entity; entity = entity->next)		
			entity->data->PostUpdate();
		for (ListItem<Entity*>* partner = partners.start; partner; partner = partner->next)
			partner->data->PostUpdate();
	}
		
	return true;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	LOG("Destroying EntityManager");

	bool ret = true;
	ClearList(ret);
	current = nullptr;
	// Unload Fx

	// Unload Tx
	app->tex->UnLoad(texEnemies);
	app->tex->UnLoad(texObstacles);
	// Unload Fx
	app->audio->Unload1Fx(fxCoffeButtons);
	app->audio->Unload1Fx(fxEnemyFound);

	// Unload Animations
	animations.Clear();
	//RELEASE(isDetectedAnim);

	score = 0;
	timeSave = 0;
	active = false;
	boxes.Clear();
	holes.Clear();
	return ret;
}

void EntityManager::ClearList(bool& ret)
{
	ListItem<Entity*>* item;
	item = entities.start;

	while (item != NULL && ret == true)
	{
		if (item->data->active == true)
			ret = item->data->CleanUp();

		item = item->next;
	}

	// Clear list
	entities.Clear();
	partners.Clear();
	spawnQueue.Clear();
	RELEASE(entityHUD);

	RELEASE(auxObstacle);
	boxes.Clear();
	holes.Clear();
	
}

void EntityManager::CheckSpawnEntities()
{
	if (spawnQueue.Count() != 0)
	{
		fPoint a;
		SDL_Rect b = app->render->camera;
		for (ListItem<Entity*>* spawnEntity = spawnQueue.start; spawnEntity; spawnEntity = spawnEntity->next)
		{
			a = spawnEntity->data->entityData.position;
			//if (((a.x > -b.x - SPAWN_MARGIN && a.x < -b.x + b.w + SPAWN_MARGIN)
			//	&& (a.y > -b.y - SPAWN_MARGIN && a.y < -b.y + b.h + SPAWN_MARGIN)) || spawnEntity->data->entityData.type == HUD
			//	|| (spawnEntity->data->entityData.type == TypeEntity::BOX_ENTITY || spawnEntity->data->entityData.type == TypeEntity::HOLE_ENTITY))
				SpawnEntity(spawnEntity->data);
		}
	}
}
void EntityManager::CheckDespawnEntities()
{
	if (entities.Count() != 0)
	{
		fPoint a;
		SDL_Rect b = app->render->camera;
		for (ListItem<Entity*>* despawnEntity = entities.start; despawnEntity; despawnEntity = despawnEntity->next)
		{
			a = despawnEntity->data->entityData.position;
			//if (!((a.x > -b.x - SPAWN_MARGIN && a.x < -b.x + b.w + SPAWN_MARGIN)
			//	&& (a.y > -b.y - SPAWN_MARGIN && a.y < -b.y + b.h + SPAWN_MARGIN))
			//	&&  (despawnEntity->data->entityData.type != TypeEntity::BOX_ENTITY && despawnEntity->data->entityData.type != TypeEntity::HOLE_ENTITY))
			//	DespawnEntity(despawnEntity->data);

			if (despawnEntity->data->entityData.state == DEAD)
			{
				app->audio->DeleteChannel(despawnEntity->data->entityData.channel);
				entities.Del(despawnEntity);
			}
		}
		for (ListItem<Entity*>* despawnPartner = partners.start; despawnPartner; despawnPartner = despawnPartner->next)
		{
			if (despawnPartner->data->entityData.state == DEAD)
			{
				app->audio->DeleteChannel(despawnPartner->data->entityData.channel);
				partners.Del(despawnPartner);
			}
		}
	}
}
bool EntityManager::AddEntity(TypeEntity pType, iPoint pos)
{
	Entity* ent = new Entity;
	ent->entityData.type = pType;
	ent->entityData.position =fPoint(pos.x,pos.y);
	spawnQueue.Add(ent);
	return true;
}

bool EntityManager::AddEntity(TypeEntity pType, int pX, int pY, int id, int level, bool move_, State state)
{
	if (state == IDLE)
	{
		Entity* b = new Entity;
		iPoint positionSpawn = app->map->MapToWorld(pX, pY);

		if (pType == NPC) positionSpawn.y -= 12;
		b->entityData.type = pType;
		b->entityData.position.x = positionSpawn.x;
		b->entityData.position.y = positionSpawn.y;
		b->entityData.positionInitial = positionSpawn;
		b->entityData.level = level;
		//b->entityData.channel = app->audio->SetChannel();
		b->entityData.id = id;
		b->move = move_;

		// Collisons
		if (pType == KENZIE_ || pType == KEILER_ || pType == ISRRA_ || pType == BRENDA_)
		{
			b->entityData.pointsCollision[0] = { 0, 0 };
			b->entityData.pointsCollision[1] = { 56, 0 };
			b->entityData.pointsCollision[2] = { 56, 92 };
			b->entityData.pointsCollision[3] = { 0, 92 };
		}
		else
		{
			b->entityData.pointsCollision[0] = { 18, 4 };
			b->entityData.pointsCollision[1] = { 48, 4 };
			b->entityData.pointsCollision[2] = { 48, 32 };
			b->entityData.pointsCollision[3] = { 18, 32 };
		}

		b->entityData.centerPoint.x = app->entity->CalculateDistance(b->entityData.pointsCollision[0], b->entityData.pointsCollision[1]) / 2;
		b->entityData.centerPoint.y = app->entity->CalculateDistance(b->entityData.pointsCollision[0], b->entityData.pointsCollision[3]) / 2;

		spawnQueue.Add(b);
	}
	return true;
}

Entity* EntityManager::FindNPC(int id)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data->entityData.type == NPC && item->data->entityData.id == id)
		{
			return item->data;
		}
	}
}

void EntityManager::SpawnEntity(Entity* info)
{
	switch (info->entityData.type)
	{
	case BANDIT:
	case FIGHTER:
	case SAPLING:
	case TRUNK:
	case PERRY:
	case ALENS:
	case LIAM:
	case NAN_:
	case BIN:
	case LICAN:
	case EESAAC:
	case HEADACHE:
		if(info->entityData.texture == nullptr)
			entities.Add(new Enemy(info, texEnemies));
		else entities.Add(new Enemy(info, info->entityData.texture));
		entities.end->data->Start();
		break;

	case KENZIE_:
	case KEILER_:
	case ISRRA_:
	case BRENDA_:
		partners.Add(new Enemy(info, info->entityData.texture));
		break;

	case HUD:
		//entities.Add(new GUI(info, texEnemies));
		entityHUD = new GUI(info, texEnemies);
		entityHUD->Start();
		//entities.end->data->Start();
		break;

	case NPC:
		entities.Add(new Enemy(info));
		entities.end->data->Start();
		break;

	case TypeEntity::BOX_ENTITY:
		auxObstacle = new ObstacleObjects(info, texObstacles);
		entities.Add(auxObstacle);
		boxes.Add(auxObstacle);
		entities.end->data->Start();
		break;

	case TypeEntity::HOLE_ENTITY:
		auxObstacle = new ObstacleObjects(info, texObstacles);
		entities.Add(auxObstacle);
		holes.Add(auxObstacle);
		entities.end->data->Start();
		break;
	}

	DeleteSpawnEntity(info);
}

void EntityManager::DespawnEntity(Entity* spawnEntity)
{ 
	spawnQueue.Add(spawnEntity);
	DeleteEntity(spawnEntity);
}

void EntityManager::DeleteEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity)
		{
			// Notify the audio manager that a channel can be released 
			//app->audio->DeleteChannel(item->data->entityData.channel);
			entities.Del(item);
			break;
		}
	}
}
void EntityManager::DeleteHUD()
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data->entityData.type == HUD)
		{
			DeleteEntity(item->data);
		}
	}
}
void EntityManager::DeleteSpawnEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = spawnQueue.start; item != NULL; item = item->next)
	{
		if (item->data == entity)
		{
			spawnQueue.Del(item);
			break;
		}
	}
}

void EntityManager::SpeedAnimationCheck(float dt)
{
	for (int i = 0; i < animations.Count(); i++)
	{
		animations.At(i)->data->speed = dt * 6;
	}
	//isDetectedAnim->speed = (dt * 1);
}

bool EntityManager::LoadState(pugi::xml_node& entityManagerNode)
{
	bool ret = true;
	pugi::xml_node entitiesNode = entityManagerNode.child("entities").first_child();
	pugi::xml_node aux;
	score = entityManagerNode.child("score").attribute("value").as_int(0);
	if (entitiesNode != NULL)
	{
		ClearList(ret);
		TypeEntity typeEn;
		iPoint pos;
		entityManagerNode.next_sibling();
		while (entitiesNode)
		{
			typeEn = (TypeEntity)entitiesNode.attribute("type").as_int();
			pos = iPoint(entitiesNode.attribute("x").as_int(), entitiesNode.attribute("y").as_int());
			if (typeEn == TypeEntity::BOX_ENTITY || typeEn == TypeEntity::HOLE_ENTITY)
			{
				AddEntity(typeEn,pos);
			}
			else 
			{
				AddEntity(typeEn, pos.x, pos.y,	entitiesNode.attribute("id").as_int(), entitiesNode.attribute("level").as_int(), entitiesNode.attribute("move").as_bool(), (State)entitiesNode.attribute("state").as_int());
			}
				entitiesNode = entitiesNode.next_sibling();
		}
	}
	
	return ret;
}
bool EntityManager::SaveState(pugi::xml_node& entityManagerNode) const
{
	if(!app->removeGame)entityManagerNode.child("score").attribute("value").set_value(score);
	else entityManagerNode.child("score").attribute("value").set_value(0);

	entityManagerNode.remove_child("entities");
	entityManagerNode.append_child("entities").set_value(0);
	ListItem<Entity*>* entiti= entities.start;
	if (!app->removeGame)
	{
		pugi::xml_node entitiesNode = entityManagerNode.child("entities");
		pugi::xml_node entity_node = entitiesNode;
		iPoint positionSpawn;
		for (entiti; entiti; entiti = entiti->next)
		{
			positionSpawn = app->map->WorldToMap(entiti->data->entityData.positionInitial.x, entiti->data->entityData.positionInitial.y);
			if(entiti->data->entityData.type == HUD) 
				positionSpawn = app->map->WorldToMap(entiti->data->entityData.position.x, entiti->data->entityData.position.y);

			entitiesNode.append_child("entity").append_attribute("type").set_value(entiti->data->entityData.type);

			if (entiti->data->entityData.type == TypeEntity::BOX_ENTITY || entiti->data->entityData.type == TypeEntity::HOLE_ENTITY)
			{
				positionSpawn = iPoint(entiti->data->entityData.position.x, entiti->data->entityData.position.y);
				positionSpawn = app->map->WorldToMap(positionSpawn);
				entitiesNode.last_child().append_attribute("x").set_value(entiti->data->entityData.position.x);
				entitiesNode.last_child().append_attribute("y").set_value(entiti->data->entityData.position.y);
			}
			else
			{
				entitiesNode.last_child().append_attribute("x").set_value(positionSpawn.x);
				entitiesNode.last_child().append_attribute("y").set_value(positionSpawn.y);
			}


			entitiesNode.last_child().append_attribute("id").set_value(entiti->data->entityData.id);
			entitiesNode.last_child().append_attribute("level").set_value(entiti->data->entityData.level);
			entitiesNode.last_child().append_attribute("state").set_value(entiti->data->entityData.state);
			entitiesNode.last_child().append_attribute("move").set_value(entiti->data->move);
			if (entiti->data->entityData.type == TypeEntity::HUD)
				entiti->data->SaveState(entityManagerNode);
		}
	}
	else
	{
		for (entiti; entiti; entiti = entiti->next)
		{
			if (entiti->data->entityData.type == TypeEntity::HUD)
				entiti->data->SaveState(entityManagerNode);
			break;
		}
	}
	return true;
}
