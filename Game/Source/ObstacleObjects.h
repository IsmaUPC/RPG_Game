#ifndef __OBSTACLE_OBJECTS_H__
#define __OBSTACLE_OBJECTS_H__

#include "Entity.h"

enum TypeObstacle 
{
	OBSTACLE_BOX,
	OBSTACLE_HOLE,

};

class ObstacleObjects :	public Entity
{
public:

	ObstacleObjects();
	ObstacleObjects(Entity* entity, SDL_Texture* tex) ;
	~ObstacleObjects();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	virtual bool Start();
	// Called each loop iteration
	virtual bool PreUpdate();

	// Called each loop iteration
	virtual bool Update(float dt);
	// Called each loop iteration
	virtual bool PostUpdate();
	// Called before quitting
	virtual bool CleanUp();

	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&) const;

public:


	iPoint tilePosition;
};

#endif // __OBSTACLE_OBJECTS_H__