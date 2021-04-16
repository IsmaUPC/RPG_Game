#include "App.h"
#include "Render.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiSettings.h"
#include "GuiManager.h"


GuiSettings::GuiSettings(iPoint Position, SceneControl* moduleObserver)
{
	active = false;

	initialPos = Position;

	SDL_Texture* btnTextureAtlas = app->guiManager->btnTextureAtlas;

	sldMusic = new GuiSlider(11, { initialPos.x, initialPos.y, 183, 40 }, "Music", 0, MIX_MAX_VOLUME, btnTextureAtlas);
	sldMusic->SetObserver(moduleObserver);
	sldMusic->active = false;
	app->guiManager->AddGuiSlider(sldMusic);

	sldFx = new GuiSlider(12, { initialPos.x, initialPos.y + padding, 183, 40 }, "Fx", 0, MIX_MAX_VOLUME, btnTextureAtlas);
	sldFx->SetObserver(moduleObserver);
	sldFx->active = false;
	app->guiManager->AddGuiSlider(sldFx);

	chBxFullScreen = new GuiCheckBox(13, { initialPos.x, initialPos.y + padding * 2, 183, 40 }, "FullScreen", app->fullScreen, btnTextureAtlas);
	chBxFullScreen->SetObserver(moduleObserver);
	chBxFullScreen->active = false;
	app->guiManager->AddGuiCheckBox(chBxFullScreen);

	chBxVSync = new GuiCheckBox(14, { initialPos.x, initialPos.y + padding * 3, 183, 40 }, "V-Sync",false, btnTextureAtlas);
	chBxVSync->SetObserver(moduleObserver);
	chBxVSync->state = GuiControlState::DISABLED;
	chBxVSync->active = false;
	app->guiManager->AddGuiCheckBox(chBxVSync);

	btnBack = new GuiButton(10, { initialPos.x, initialPos.y + padding * 4, 183, 91 }, "Back", BACK,btnTextureAtlas);
	btnBack->SetObserver(moduleObserver);
	btnBack->active = false;
	app->guiManager->AddGuiButton(btnBack);

}

GuiSettings::~GuiSettings()
{

}

bool GuiSettings::Update(float dt)
{
	return true;
}

bool GuiSettings::Draw()
{

	app->render->DrawTextBox(initialPos.x, initialPos.y, 307, 320, { 24, 61, 172 }, { 97, 159, 207 }, { 0, 33, 78 }, app->guiManager->moonCorner, 200);

	return true;
}

bool GuiSettings::CleanUp()
{
	return true;
}

void GuiSettings::AbleDisableSetting()
{
	active = !active;
	sldMusic->active = active;
	sldFx->active = active;
	chBxFullScreen->active = active;
	chBxVSync->active = active;
	btnBack->active = active;
}

void GuiSettings::MovePosition()
{
	
	int x = initialPos.x - app->render->camera.x;
	int y = initialPos.y - app->render->camera.y;

	sldMusic->bounds.x = x;
	sldMusic->bounds.y = y;

	sldFx->bounds.x = x;
	sldFx->bounds.y = y + padding * 1;

	chBxFullScreen->bounds.x = x;
	chBxFullScreen->bounds.y = y + padding * 2;

	chBxVSync->bounds.x = x;
	chBxVSync->bounds.y = y + padding * 3;

	btnBack->bounds.x = x+(183/4);
	btnBack->bounds.y = y + padding * 4;

}

