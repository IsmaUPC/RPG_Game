#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"
#include "Audio.h"

struct Stats
{
	int attack = 0;
	int defense = 0;
	int speed = 0;
	int health = 0;
	int mana = 0;
	float exp = 0;
};

class Enemy : public Entity
{
public:
	Enemy();
	Enemy(Entity* entity, SDL_Texture* tex);

	virtual ~Enemy();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool CheckCollisionEnemy(fPoint nextPosition);

	bool PreUpdate();

	void CheckCollisionEnemyToPlayer();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

private:

	bool Radar(iPoint origin, int range);
	void CreatePathEnemy(iPoint origin, iPoint destination);
	int GetCurrentPositionInPath(iPoint mapPositionEnemy);
	void MoveEnemy();
	int CalculateDistance(iPoint origin, iPoint destination);

private:

	int range = 150;
	int rangeMax = 200;
	bool isDetected = false;
	bool returning = false;
	int radiusCollision = 0;
	
	iPoint destination;
	fPoint tmp;

	// We store the created path here
	DynArray<iPoint>* lastPath;
public:
	// Stats
	Stats stats;
};
#endif // _ENEMY_H_
