#ifndef __SCENEINTRO_H__
#define __SCENEINTRO_H__

#include "Scene.h"
#include "Animation.h"
#include "GuiButton.h"
#include "GuiSlider.h"
#include "GuiSettings.h"

struct SDL_Texture;

class SceneIntro : public SceneControl
{
public:

	SceneIntro(SceneType type);

	// Destructor
	virtual ~SceneIntro();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	void CloudsUpdate();

	// Called before all Updates
	bool PostUpdate();

	void CloudsDraw();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

	void CloaseMenuSettings();

	bool LoadState(pugi::xml_node& data);
	bool SaveState(pugi::xml_node& data)const;

	void ComprobeState(int id);
	void AbleButtons();

private:

	SDL_Texture* bgIntro = nullptr;
	SDL_Texture* logo = nullptr;
	SDL_Texture* cloud = nullptr;
	SDL_Texture* oneiric = nullptr;
	SDL_Texture* exoria = nullptr;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;

	bool transition = false;
	bool closeSettings = false;

	fPoint sBackCloudPos = { 0,0 };
	fPoint bBackCloudPos = { 0,0 };

	fPoint bCloudPos = { 0,0 };
	fPoint bCloudPos2 = { 0,0 };
	fPoint sCloudPos = { 0,0 };
	fPoint sCloudPos2 = { 0,0 };

	pugi::xml_document sceneFile;
	pugi::xml_node sceneStateFile;

	GuiButton* btnPlay = nullptr;
	GuiButton* btnContinue = nullptr;
	GuiButton* btnSettings = nullptr;
	GuiButton* btnExit = nullptr;

	GuiSettings* menuSettings = nullptr;
	uint fxStart = -1;
	uint fxExit = -1;
	uint fxTittle = -1;
	uint fxFlash = -1;

	//Easings title
	float currentIteration;
	float totalIterations;
	float initialPosition;
	float deltaPosition;

	float positionOneiric = -500;
	float positionExoria = -500;

	bool flash = false;
	float logoAlpha = 0;
	float timeCounter = 0;
	int state = 0;
	float angle = 0;
};

#endif // __SCENEINTRO_H__



