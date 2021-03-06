﻿#ifndef __GUICONTROL_H__
#define __GUICONTROL_H__

#include "Input.h"
#include "Render.h"
#include "SceneControl.h"
#include "SString.h"

#include "SDL/include/SDL.h"
#include "SDL_ttf/include/SDL_ttf.h"

enum class GuiControlType
{
	BUTTON,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class GuiControlState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};

class GuiControl
{
public:

	GuiControl(GuiControlType type, uint32 id) : type(type), id(id), state(GuiControlState::NORMAL) {}

	GuiControl(GuiControlType type, SDL_Rect bounds, const char* text) :
		type(type),
		state(GuiControlState::NORMAL),
		bounds(bounds),
		text(text) 
	{
		color.r = 255; color.g = 255; color.b = 255;
		texture = NULL;
	}

	virtual bool Update(Input* input, float dt)
	{
		return true;
	}

	virtual bool Draw(Render* render) const
	{
		return true;
	}

	void SetTexture(SDL_Texture* tex)
	{
		texture = tex;
		section = { 0, 0, 0, 0 };
	}

	void SetObserver(SceneControl* module)
	{
		observer = module;
	}

	bool NotifyObserver()
	{
	   return observer->OnGuiMouseClickEvent(this);
	}

public:

	uint32 id = 0;
	GuiControlType type;

   // TypeButton typeButton= RECTANGLE;
	GuiControlState state;

	SString text;		   // Control text (if required)
	SDL_Rect bounds;		// Position and size
	SDL_Color color;		// Tint color

	SDL_Texture* texture;   // Texture atlas reference
	SDL_Texture* handCursor; // Hand cursor texture
	SDL_Rect section;	   // Texture atlas base section

	TTF_Font* font;			// Text font
	bool active = true;

	SceneControl* observer = nullptr;	// Observer module (it should probably be an array/list)
};

#endif // __GUICONTROL_H__