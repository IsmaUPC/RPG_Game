#include "Entity.h"
#include "Player.h"
#include "EntityManager.h"
#include "TpNodeManager.h"
#include "TeleportNode.h"
#include "SceneManager.h"

#include "Defs.h"
#include "Log.h"

Entity::Entity(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture) : Module()
{
	name.Create("Entity");
	isAlive = true;
}

Entity::Entity() : Module()
{
	name.Create("Entity");
	isAlive = true;
}

// Destructor
Entity::~Entity()
{}

// Called before Entity is available
bool Entity::Awake(pugi::xml_node& config)
{
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Entity::Start()
{
	LOG("Entity start");
	// back background
	return true;
}

// Called each loop iteration
bool Entity::PreUpdate()
{
	return true;
}

bool Entity::Update(float dt)
{
	return true;
}

bool Entity::PostUpdate()
{

	return true;
}

// Called before quitting
bool Entity::CleanUp()
{
	LOG("Destroying Entity");
	active = false;

	return true;
}

// Load Game State
bool Entity::LoadState(pugi::xml_node& data)
{
	return true;
}

// Save Game State
bool Entity::SaveState(pugi::xml_node& data) const
{
	return true;
}
iPoint Entity::TransformFPoint(fPoint fpoint)
{
	iPoint transformation;

	transformation.x = fpoint.x;
	transformation.y = fpoint.y;

	return transformation;
}
iPoint Entity::MapToWorld(iPoint position)
{
	app->entityManager->mapDimensions = app->map->GetDimensionsMap();
	iPoint ret;
	ret.x = position.x * app->entityManager->mapDimensions.x;
	ret.y = position.y * app->entityManager->mapDimensions.y;
	return ret;
}
int Entity::CalculateDistance(iPoint origin, iPoint destination)
{
	return abs(origin.x - destination.x) + abs(origin.y - destination.y);
}
// Comprove position player in array of tiles in mapLayer collision
int Entity::CheckCollision(iPoint positionMap)
{
	uint typeTilePlayer = app->map->data.layers.At(7)->data->Get(positionMap.x, positionMap.y);
	uint firstgidLayerCollisions = app->map->data.tilesets.At(0)->data->firstgid;
	typeTilePlayer -= firstgidLayerCollisions;

	if (app->player->godMode == false && app->player->win != true)
	{
		switch (typeTilePlayer)
		{
		case VICTORY:
			// Victory
			if(positionMap == app->map->WorldToMap( app->player->playerData.position))
			app->player->win = true;
			return VICTORY;
			break;

		case COLLISION:
			// Collision
			return COLLISION;
			break;
		
		case UP_LADDER:
		case DOWN_LADDER:
		case UP_HALL:
		case DOWN_HALL:
			// Fill node origin in Scene manager
			app->sceneManager->originTpNode = app->sceneManager->tpManager->FindNodeTpInPlayer(typeTilePlayer);
			app->player->win = true;
			break;

		case CHECK_POINT:
			// Checkpoint
			//if (positionMap == app->map->WorldToMap(app->player->playerData.position))
			//app->player->ActiveCheckpoint(positionMap);
			return CHECK_POINT;
			break;

		case HOLE:
			return HOLE;
			break;
		case BOX:
			return BOX;
		break;

		default:
			return -1;
			break;
		}
	}

	return false;
}
bool Entity::CheckCollisionObstacle(iPoint positionMap)
{

	for (int i = 0; i < app->entityManager->boxes.Count(); i++)
		if (positionMap == app->entityManager->boxes.At(i)->data->tilePosition)
			return true;

	for (int i = 0; i < app->entityManager->holes.Count(); i++)
		if (positionMap == app->entityManager->holes.At(i)->data->tilePosition)
			return true;
	
	return false;
}
bool Entity::CheckCollisionBoxes(iPoint positionMap)
{
	for (int i = 0; i < app->entityManager->boxes.Count(); i++)
		if (positionMap == app->entityManager->boxes.At(i)->data->tilePosition)
			return true;

	return false;
}