#ifndef __DIALOGSYSTEM_H__
#define __DIALOGSYSTEM_H__

#include "App.h"
#include "Log.h"
#include <string>
#include <iostream>
#include <vector>
#include "Render.h"
#include "Textures.h"
#include "Module.h"
#include "Input.h"
#include "GuiButton.h"
#include "SDL/include/SDL.h"

using namespace std;

class DialogueNode;

class DialogueOption
{
public:
	DialogueOption() {};
	~DialogueOption() {};

	DialogueOption(string Text, int ReturnCode, int NextNode);
	string text;
	int returnCode;
	int nextNode;
};

class DialogueNode
{
public:
	DialogueNode() {};
	~DialogueNode() {};

	DialogueNode(string Text);
	string text;
	vector <DialogueOption*> dialogueOptions;
	List <string> answersList;
	int nodeId;
};

class DialogueTree
{
public:
	DialogueTree() {};
	~DialogueTree() {};


public:
	int treeId;
	vector <DialogueNode*> dialogueNodes;
};

class DialogueSystem : public Module
{
public:
	DialogueSystem();
	DialogueSystem(Input* input, Render* render, Textures* tex)
	{
		this->input = input;
		this->render = render;
		this->tex = tex;
	}
	~DialogueSystem();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void PerformDialogue(int treeId, int playerInput);
	bool LoadDialogue(const char*);
	bool LoadNodes(pugi::xml_node& trees, DialogueTree* oak);
	bool LoadOptions(pugi::xml_node& text_node, DialogueNode* npc);
	DialogueNode* currentNode;
	std::vector <DialogueTree*> dialogueTrees;
	bool onDialog = false;
	SDL_Texture* moonCorner;
	void SetId(int id_) { id = id_; };
	int id = 0;
	bool missClick = false;

private:
	//int playerInput = 7;
	//DialogueNode* currentNode;

	iPoint point = { 0,0 };
	pugi::xml_document	dialogues;

	Input* input;
	Render* render;
	Textures* tex;

};
#endif // __DIALOGSYSTEM_H__

