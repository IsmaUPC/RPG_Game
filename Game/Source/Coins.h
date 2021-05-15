#ifndef __COINS_H__
#define __COINS_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"



class Coins : public Entity
{
public:

	Coins(iPoint pos);
	Coins(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture);

	virtual ~Coins();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();
	void CurrentCoinAnimation();


public:

	bool isCollected = false;
private:
	iPoint position = { 0,0 };
	int numPoints = 0;
	iPoint* pointsCollision = nullptr;

	Animation* coinAnimation = new Animation();
	Animation* particleAnimation = new Animation();

	uint coinFx = -1;

	SDL_Texture* texCoin = nullptr;
	//SDL_Texture* texCoinParticle;
};
#endif // _COINS_H_

