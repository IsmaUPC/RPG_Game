#ifndef __SCENE_H__
#define __SCENE_H__

#include "SceneControl.h"
#include "Animation.h"
#include "GuiManager.h"

#include "PugiXml\src\pugixml.hpp"

struct SDL_Texture;

class Scene : public SceneControl
{
public:

	Scene(SceneType type) ;

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	void SetDebugCollaider();
	bool* GetDebugCollaider() { return &debugCollisions; }
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	void UpdateDialog();

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	int GetNumThisScene()
	{
		return numThisScene;
	}

	bool OnGuiMouseClickEvent(GuiControl* control);

	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;
	

private:

	int numThisScene = 1;

	void DebugKeys();

	bool debugCollisions = false;

	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	float speedImg=0;

	GuiButton* btn1 = nullptr;
	GuiButton* btn2 = nullptr;
	GuiButton* btn3 = nullptr;
};

#endif // __SCENE_H__