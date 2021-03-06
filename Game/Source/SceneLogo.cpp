﻿#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "SceneLogo.h"
#include "Audio.h"

#include "Defs.h"
#include "Log.h"

#define LOGO_FADE_SPEED 500.0f

SceneLogo::SceneLogo(SceneType type) : SceneControl(type)
{
	active = true;
	name.Create("sceneLogo");
}

SceneLogo::~SceneLogo()
{
}

bool SceneLogo::Awake()
{
	LOG("Loading SceneLogo");
	bool ret = true;
	return ret;
}

bool SceneLogo::Start()
{
	app->SetLastScene((Module*)this);

	logo = app->tex->Load("Textures/logo_7hd.png");
	cracks = app->tex->Load("Textures/cracks_logo.png");
	fxIntro = app->audio->LoadFx("Audio/Fx/logo.wav");


	SDL_QueryTexture(logo, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	logoRect = {imgX,imgY,imgW,imgH};

	state = 0;
	timeCounter = 0.0f;
	logoAlpha = 0.0f;
	scale = 4;

	// Background color
	color.r = 20; color.g = 20; color.b = 20;
	
	return true;
}

bool SceneLogo::PreUpdate()
{
	return true;
}

bool SceneLogo::Update(float dt)
{
	if (state == 0)
	{
		state = 1;
	}
	else if (state == 1)
	{
		logoAlpha += (LOGO_FADE_SPEED * dt);

		if (logoAlpha > 255.0f)
		{
			logoAlpha = 255.0f;
			state = 2;
		}
	}
	else if (state == 2)
	{
		// Waiting for 3 seconds
		timeCounter += dt;
		if (timeCounter >= 2.5f) state = 3;
	}
	else if (state == 3)
	{
		if(logoAlpha!=0)logoAlpha -= (LOGO_FADE_SPEED * dt);

		if (logoAlpha <= 0.0f)
		{
			logoAlpha = 0.0f;
			TransitionToScene(SceneType::INTRO);
		}
	}

	
	if (scale > 1)
	{
		SDL_QueryTexture(logo, NULL, NULL, &logoRect.w, &logoRect.h);
		logoRect.w *= scale;
		logoRect.h *= scale;
		scale -= 0.1166;
	}
	else if (!cracksOn)
	{
		cracksOn = true;
		app->audio->PlayFx(fxIntro);
	}

	return true;
}

bool SceneLogo::PostUpdate()
{
	app->render->SetBackgroundColor(color);

	bool ret = true;

	if (scale <= 1)
	{
		app->render->DrawTexture(cracks, WINDOW_W / 2 - logoRect.w / 2 - 40, WINDOW_H / 2 - logoRect.h / 2 - 40);
		SDL_SetTextureAlphaMod(cracks, logoAlpha);
		SDL_SetTextureAlphaMod(logo, logoAlpha);
	}
	app->render->DrawTexture(logo, WINDOW_W / 2 - logoRect.w / 2, WINDOW_H / 2 - logoRect.h / 2 , 0, scale);

	return ret;
}

bool SceneLogo::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	app->tex->UnLoad(logo);
	app->tex->UnLoad(cracks);
	app->tex->UnLoad(logo);
	app->audio->Unload1Fx(fxIntro);

	logo = nullptr;
	cracks = nullptr;
	active = false;

	return true;
}

