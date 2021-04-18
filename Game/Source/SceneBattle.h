#ifndef __SCENE_BATTLE_H__
#define __SCENE_BATTLE_H__

#include "SceneControl.h"
#include "GuiButton.h"
#include "Animation.h"
#include "GuiMenuMagic.h"
#include "EntityManager.h"

#include "PugiXml\src\pugixml.hpp"
#include <string>

struct Magic
{
	int id = 0;
	int type = 0;
	int level = 0;
	SString name = "";
	int damage = 0;
	int mana = 0;
	SString description = "";
};

enum FaseAttack
{
	SELECT_ACTION,
	SELECT_ENEMY,
	DO_ACITON,
	END_ACTION,
	ENEMY_ATTACK
};

struct SDL_Texture;
class SceneBattle : public SceneControl
{
public:
	SceneBattle();

	// Destructor
	virtual ~SceneBattle();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();
	void AddBattleMenu(SDL_Texture* btnTextureAtlas);
	void LoadAnimations();
	void InicializeStats();
	void AddPartners();
	void AddEntities();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();
	void DrawBarLives();
	void DrawBarExperience();
	void DrawTurnBar();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);
	int CalculateExp(int level);
	void AssignEntities();
	void SpeedAnimationCheck(float dt);

	// Turn bar
	void BubbleSort();
	void DisplaceToLeft();

	void AbleDisableButtons();
	
	void SaveStateLose();
	void SaveState(TypeEntity pType, int i);

private:

	void UseAMagic();

	SDL_Rect rec = { 0,0,0,0 };
	SDL_Rect live = { 0,0,0,0 };
	SDL_Rect tex = { 0,0,0,0 };
	SDL_Rect face = { 0,0,0,0 };

	SDL_Color green = { 0,187, 45 };
	SDL_Color yellow = { 229,190,1 };
	SDL_Color red = { 203,50,52 };
	SDL_Color blue = {37, 40,80};
	SDL_Color cyan = { 0, 183,255 };
	SDL_Color orange = {255,136,18};
	SDL_Color white = { 255,255,255 };
	SDL_Color black = { 0,0,0 };

	GuiButton* btnAttack = nullptr;
	GuiButton* btnMagic = nullptr;
	GuiButton* btnDefense = nullptr;
	GuiButton* btnExit = nullptr;
	GuiButton* btnContinue = nullptr;

	SDL_Texture* img = nullptr;
	SDL_Texture* texPalyers = nullptr;
	SDL_Texture* texEnemies = nullptr;

	List<Animation*> spritesBarTurn;
	List<Animation*> animationsPlayer;
	List<Animation*> animationsHitPlayer;
	List<Animation*> animationsEnemies;
	List<Animation*> animationsHitEnemies;
	List<Animation*> animationsAttackEnemies;

	List<Entity*> enemies;
	List<Entity*> partners;
	Entity* turnSort = nullptr;

	uint winFx = -1;
	uint loseFx = -1;
	uint attackFx = -1;
	uint magicFx = -1;
	uint defenseFx = -1;
	uint exitFx = -1;


	GuiMenuMagic* menuMagic = nullptr;
	pugi::xml_document saveFile;
	
	Magic* magicInUse = nullptr;
	bool activeMenuMagic =false;

	int turn = 0;
	int tam = 0;
	int numArrows = 0;
	float dt = 0;
	float offset = 38;
	char textVictory[9] = { "VICTORY!" };
	char textLoose[10] = { "YOU LOOSE" };
	char textLive[8] = { "\0" };
	char textLevel[7] = { "\0" };
	char textExperience[14] = { "\0" };

	int faseAction = 0;
	int enemySelected = 0;
	int ally=0;
	bool hit=0;
	float newHealth = 0;
	int reduceLieveVelocity = 8;

	bool moveBarTurn = false;
	bool assigneDone = false;
	bool win = false;
	bool lose = false;
	bool god = false;
	int totalExp = 0;
	float currentExp = 0;
	
	bool missClick = false;
	bool initCombat = true;
	int indexTurnBar = -1;

	int posX = 0;
	int posY = 0;
};
#endif //__SCENE_BATTLE_H__