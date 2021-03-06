#ifndef __GUISETTINGS_H__
#define __GUISETTINGS_H__

#include "SString.h"
#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSlider.h"


class GuiSettings
{
public:

	GuiSettings(iPoint Position, SceneControl* moduleObserver);
	
	~GuiSettings();

	bool Update(float dt);
	void AbleDisableButtons();
	bool Draw();

	bool CleanUp();

	void AbleDisableSetting();
	bool GetActiveSettings() { return active; };

	void DesactiveSettingMenu();

	void MovePosition();


	GuiSlider* sldMusic = nullptr;
	GuiSlider* sldFx = nullptr;
	GuiCheckBox* chBxFullScreen = nullptr;
	GuiCheckBox* chBxVSync = nullptr;
	GuiCheckBox* chBxTextSpeed0 = nullptr;
	GuiCheckBox* chBxTextSpeed1 = nullptr;
	GuiCheckBox* chBxTextSpeed2 = nullptr;

	GuiButton* btnBack = nullptr;

	bool active = false;
	bool pendingDesactive = false;

private:
	int padding = 60;

	char textSpeedOptions[20] = { "Slow Fast Off" };

	iPoint initialPos = { 0,0};

	// Easings variables
	int currentIteration = 0;
	int totalIterations = 60;
	int hight = 10;

};
#endif // __GUISETTINGS_H__



