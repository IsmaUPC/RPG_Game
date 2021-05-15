#include "SceneManager.h"
#include "GuiManager.h"

#include "SceneLogo.h"
#include "SceneIntro.h"
#include "Scene.h"
#include "SceneLevel2.h"
#include "SceneLevel3.h"
#include "SceneDungeon.h"
#include "SceneLose.h"
#include "SceneWin.h"
#include "SceneBattle.h"
#include "Fonts.h"

#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "GuiManager.h"
#include "TpNodeManager.h"
#include "TransitionManager.h"

#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"

#define FADEOUT_TRANSITION_SPEED	2.0f
#define FADEIN_TRANSITION_SPEED		2.0f

SceneManager::SceneManager(Input* input, Render* render, Textures* tex) : Module()
{
	name.Create("scene_manager");

	onTransition = false;
	fadeOutCompleted = false;
	transitionAlpha = 0.0f;

	this->input = input;
	this->render = render;
	this->tex = tex;
}

// Destructor
SceneManager::~SceneManager()
{}

// Called before render is available
bool SceneManager::Awake()
{
	LOG("Loading Scene manager");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool SceneManager::Start()
{
	current = new SceneLogo();
	current->Start();

	tpManager = new TpNodeManager();

	guiFont = app->fonts->Load("Assets/Fonts/RPGSystem.ttf", 25);
	titleFont = app->fonts->Load("Assets/Fonts/title_font.ttf", 48);
	statsFont = app->fonts->Load("Assets/Fonts/title_font.ttf", 32);
	runicFont = app->fonts->Load("Assets/Fonts/runic_font.ttf", 15);

	transitionFx = app->audio->LoadFx("Assets/Audio/Fx/combat_transition.wav");

	texPlayers = app->tex->Load("Assets/Textures/Characters/atlas_players_battle.png");

	LoadTmxDungeonsList();

	next = nullptr;

	return true;
}

// Called each loop iteration
bool SceneManager::PreUpdate()
{
	if (input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) ViewRectangles = !ViewRectangles;
	if (enemyDetected && !onTransition)	current->TransitionToScene(SceneType::BATTLE);

	return true;
}

// Called each loop iteration
bool SceneManager::Update(float dt)
{
	bool ret = true;
	if (!pause && (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].start) 
		&& (current->name == "scene" || current->name == "sceneLevel2" || current->name == "sceneLevel3"))
	{
		currentVolume = app->audio->GetVolumeMusic();
		if (currentVolume > 10) app->audio->SetVolumeMusic(10); 
		pause = !pause;
		app->guiManager->GetMenuPause()->AbleDisableMenu();
		app->fonts->ResetH();
	}

	if (!pause && (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN || app->input->pads[0].back)
		&& (current->name == "scene" || current->name == "sceneLevel2" || current->name == "sceneLevel3"))
	{
		pause = !pause;
		app->player->playerData.state = MOBILE;
	}

	if (!onTransition)
	{
		ret = current->Update(dt);
	}
	else
	{
		if (!fadeOutCompleted)
		{
			transitionAlpha += (FADEOUT_TRANSITION_SPEED * dt);

			// NOTE: Due to float internal representation, condition jumps on 1.0f instead of 1.05f
			// For that reason we compare against 1.01f, to avoid last frame loading stop
			if (transitionAlpha > 1.01f && app->transitionManager->GetTransitionMid())
			{
				transitionAlpha = 1.0f;

				current->CleanUp();	// Unload current screen
				app->guiManager->DeleteList();
				next->Start();	// Load next screen

				//Spawn Player in node TP
				tpManager->SpawnPlayerTpNode(originTpNode);
				//originTpNode = nullptr;
				app->player->RePositionPartners();


				if (current->isContinue) app->LoadGameRequest();
				else if (next->name == "scene" || next->name == "sceneLevel2") // AutoSave
				{
					app->SaveGameRequest();
				}
				RELEASE(current);	// Free current pointer
				current = next;		// Assign next pointer
				next = nullptr;

				// Menu pause
				app->guiManager->CreatMenuPause(current);
				if(current->name == "scene" || current->name == "sceneLevel2" || current->name == "sceneLevel3")
					app->guiManager->CreateStatsMenu(current);

				// Activate fade out effect to next loaded screen
				fadeOutCompleted = true;
			}
		}
		else  // Transition fade out logic
		{
			transitionAlpha -= (FADEIN_TRANSITION_SPEED * dt);

			if (transitionAlpha < -0.01f)
			{
				transitionAlpha = 0.0f;
				fadeOutCompleted = false;
				onTransition = false;
				app->fonts->ResetH();
			}
		}
	}

	if (current->transitionRequired)
	{
		onTransition = true;
		fadeOutCompleted = false;
		transitionAlpha = 0.0f;

		switch (current->nextScene)
		{
		case SceneType::LOGO: next = new SceneLogo(); break;
		case SceneType::INTRO: next = new SceneIntro(); break;
		case SceneType::LEVEL1: next = new Scene(); break;
		case SceneType::LEVEL2: next = new SceneLevel2(); break;
		case SceneType::LEVEL3: next = new SceneLevel3(); break;
		case SceneType::DUNGEON: 
			//if (current->name != "dungeon")
				next = new SceneDungeon();
			//else 
				//next = current;
			break;

		case SceneType::WIN: next = new SceneWin(); break;
		case SceneType::LOSE: next = new SceneLose(); break;
		case SceneType::BATTLE: next = new SceneBattle(); app->audio->PlayFx(transitionFx); break;
		default: break;
		}

		current->transitionRequired = false;
	}
	
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		current->TransitionToScene(SceneType::LEVEL1);
		lastLevel = 1;
		originTpNode = nullptr;
	}
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		current->TransitionToScene(SceneType::LEVEL2);
		lastLevel = 2;
		originTpNode = nullptr;
	}
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		current->TransitionToScene(SceneType::LEVEL3);
		lastLevel = 3;
		originTpNode = nullptr;
	}
	if (app->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{
		current->TransitionToScene(SceneType::DUNGEON);
		lastLevel = 8;
		originTpNode = nullptr;
	}

	return ret;
}

// Called each loop iteration
bool SceneManager::PostUpdate()
{
	bool ret = true;

	if (app->player->win== true)
	{

		app->player->win = false;
		current->victory = false;

		if(!onTransition)
		switch (originTpNode->typeTpNode)
		{
		case UNKNOW:
			break;
		case DOWN_LADDER_NODE:
			if (originTpNode->idFloor <= 0)originTpNode->idFloor = 1;
			if (levelDungeon == 0)
				current->TransitionToScene(SceneType(((int)SceneType::LEVEL1 + originTpNode->idFloor) - 1));
			else
			{
				if (SceneType(((int)SceneType::LEVEL1 + originTpNode->idFloor)) == SceneType::DUNGEON)
					levelDungeon--;
				current->TransitionToScene(SceneType::DUNGEON);

			}
			break;
		case UP_LADDER_NODE:
			if (SceneType(((int)SceneType::LEVEL1 + originTpNode->idFloor) ) == SceneType::DUNGEON)
				levelDungeon++;
			if ((int)SceneType::LEVEL1 + originTpNode->idFloor != (int)SceneType::DUNGEON)
				current->TransitionToScene(SceneType(((int)SceneType::LEVEL1 + originTpNode->idFloor) + 1));
			else 
			{
				current->TransitionToScene(SceneType::DUNGEON);

			}
			return true;
			break;
		default:
			break;
		}

	}
	if (current->lose == true)
	{
		current->lose = false;
		current->TransitionToScene(SceneType::LOSE);
		return true;
	}

	// Draw current scene
	current->PostUpdate();
	// Draw full screen rectangle in front of everything
	if (onTransition)
	{
		//render->DrawRectangle({ -app->render->camera.x, -app->render->camera.y, WINDOW_W, WINDOW_H }, 0, 0, 0, (unsigned char)(255.0f * transitionAlpha));
	}	

	return ret;
}

void SceneManager::AddScene(SceneControl* scene, bool active)
{
	scene->active = active;
	scenes.Add(scene);
}

// Called before quitting
bool SceneManager::CleanUp()
{
	LOG("Freeing scene");
	if (current != nullptr) current->CleanUp();

	app->fonts->UnLoad(0);
	app->fonts->UnLoad(1);
	app->fonts->UnLoad(2);
	app->fonts->UnLoad(3);
	app->tex->UnLoad(texPlayers);
	app->audio->Unload1Fx(transitionFx);

	input = nullptr;
	render = nullptr;
	tex = nullptr;
	current = nullptr;
	next = nullptr;
	transition = nullptr;
	texPlayers = nullptr;

	scenes.Clear();

	return true;
}

bool SceneManager::LoadState(pugi::xml_node& data)
{
	if (current->lastLevel == 1)current = scene;
	else if (current->lastLevel == 2)current = sceneLevel2;
	else if (current->lastLevel == 3)current = sceneLevel3;

	//TODO dungeon
	current->LoadState(data);
	return true;
}

bool SceneManager::SaveState(pugi::xml_node& data) const
{
	current->SaveState(data);
	return true;
}

bool SceneManager::LoadTmxDungeonsList()
{

	//tmxDungeonsList;
	pugi::xml_parse_result result = stateFile.load_file(DUNGEONS_FILE);

	bool ret = true;
	pugi::xml_node tmxDungeon = stateFile.child("dungeons").first_child();
	pugi::xml_node aux;

	if (tmxDungeon != NULL)
	{
			tmxDungeonsList.Clear();

		tmxDungeon.next_sibling();
		while (tmxDungeon)
		{
			tmxDungeonsList.Add( tmxDungeon.attribute("name").as_string());
			tmxDungeon = tmxDungeon.next_sibling();
		}
	}

	return ret;
}
