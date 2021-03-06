#include "DialogSystem.h"

#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Scene.h"
#include "Audio.h"
#include "QuestManager.h"

#include "SDL/include/SDL.h"


DialogueSystem::DialogueSystem() {}

DialogueSystem::~DialogueSystem() {}

bool DialogueSystem::Start()
{
	LoadDialogue(DIALOGUE_TREE_FILENAME);
	fxDialog = app->audio->LoadFx("Audio/Fx/dialogue.wav");

	return true;
}

bool DialogueSystem::Update(float dt)
{
	return true;
}

bool DialogueSystem::PostUpdate()
{
	point.x = -app->render->camera.x;
	point.y = -app->render->camera.y;

	if (onDialog == true) 
	{
		DrawDialogue();

		DrawOptions();
	}

	return true;
}


bool DialogueSystem::CleanUp()
{
	for (int i = 0; i < dialogueTrees.size(); i++)
	{
		for (int j = 0; j < dialogueTrees[i]->dialogueNodes.size(); j++)
		{
			dialogueTrees[i]->dialogueNodes[j]->answersList.Clear();
			dialogueTrees[i]->dialogueNodes[j]->dialogueOptions.clear();
		}
		dialogueTrees[i]->dialogueNodes.clear();
	}
	
	dialogueTrees.clear();
	app->audio->Unload1Fx(fxDialog);

	return true;
}

void DialogueSystem::PerformDialogue(int treeId, int playerInput)
{
	if (playerInput >= 0 && playerInput < currentNode->dialogueOptions.size())
	{
		for (int i = 0; i < dialogueTrees[treeId]->dialogueNodes.size(); i++)
			if (currentNode->dialogueOptions[playerInput]->nextNode == dialogueTrees[treeId]->dialogueNodes[i]->nodeId)
			{
				if (currentNode->dialogueOptions[playerInput]->completeID != 0)
				{
					app->questManager->FinishQuest(currentNode->dialogueOptions[playerInput]->completeID);
				}
				if (currentNode->dialogueOptions[playerInput]->activeID != 0)
				{
					app->questManager->ActiveQuest(currentNode->dialogueOptions[playerInput]->activeID);
				}
				if (currentNode->dialogueOptions[playerInput]->nextNode == 100)
				{
					spawnDialog = false;
					despawnDialog = true;
					app->dialogueSystem->onDialog = false;
				}
				else currentNode = dialogueTrees[treeId]->dialogueNodes[i];
				break;
			}
	}
}

void DialogueSystem::DrawDialogue()
{
	char NPCdialogue[420] = { 0 };
	char drawNPCdialogue[420] = { 0 };

	sprintf_s(NPCdialogue, 420, currentNode->text.c_str(), 56);

	if (dialogSpeed == 0) app->fonts->BlitMarginText(point.x + WINDOW_W / 2 - 300 + 45, point.y + 600, 0, NPCdialogue, { 60, 43, 13 }, 520);
	else if (dialogSpeed == 1)
	{
		if (actualLetter == 0)
		{
			app->audio->PlayFx(fxDialog, 1);
		}
		totalLetters = strlen(NPCdialogue);

		if (actualLetter <= totalLetters) actualLetter += 0.5;

		for (int i = 0; i < actualLetter; i++)
		{
			drawNPCdialogue[i] = NPCdialogue[i];
		}

		app->fonts->BlitMarginText(point.x + WINDOW_W / 2 - 300 + 45, point.y + 600, 0, drawNPCdialogue, { 60, 43, 13 }, 520);
	}
	else if (dialogSpeed == 2)
	{
		if (actualLetter == 0)
		{
			app->audio->PlayFx(fxDialog, 1);
		}
		totalLetters = strlen(NPCdialogue);

		if (actualLetter <= totalLetters) actualLetter++;

		for (int i = 0; i < actualLetter; i++)
		{
			drawNPCdialogue[i] = NPCdialogue[i];
		}

		app->fonts->BlitMarginText(point.x + WINDOW_W / 2 - 300 + 45, point.y + 600, 0, drawNPCdialogue, { 60, 43, 13 }, 520);
	}

	if (actualLetter >= totalLetters - 1)
	{
		app->audio->StopFx(1);
	}
}

void DialogueSystem::DrawOptions()
{
	char response[128] = { 0 };
	for (int i = 0; i < currentNode->answersList.Count(); i++)
	{
		sprintf_s(response, 128, currentNode->answersList.At(i)->data.c_str(), 56);
		//app->fonts->BlitText(point.x + WINDOW_W / 2 - 400 + (175 * (i + 1)), point.y + 675, 0, response, { 60, 43, 13 });
	}
}

bool DialogueSystem::LoadDialogue(const char* file)
{
	int size = app->assets->MakeLoad(file);

	pugi::xml_parse_result result = dialogues.load_buffer(app->assets->GetLastBuffer(), size);
	app->assets->DeleteBuffer();

	bool ret = true;

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file, result.description());
		ret = false;
	}
	else
	{
		pugi::xml_node n = dialogues.first_child().child("dialogueExample");

		for (n; n != NULL; n = n.next_sibling("dialogueExample"))
		{
			DialogueTree* tree = new DialogueTree;
			tree->treeId = n.attribute("Id").as_int();
			LoadNodes(n, tree);
			dialogueTrees.push_back(tree);
		}
	}
	return true;
}

bool DialogueSystem::LoadNodes(pugi::xml_node& trees, DialogueTree* example)
{
	for (pugi::xml_node n = trees.child("node"); n != NULL; n = n.next_sibling("node"))
	{
		DialogueNode* node = new DialogueNode;
		node->text.assign(n.attribute("text").as_string());
		node->nodeId = n.attribute("id").as_int();
		LoadOptions(n, node);
		example->dialogueNodes.push_back(node);
	}

	return true;
}

bool DialogueSystem::LoadOptions(pugi::xml_node& response, DialogueNode* answers)
{
	for (pugi::xml_node option = response.child("dialogue"); option != NULL; option = option.next_sibling("dialogue"))
	{
		DialogueOption* selection = new DialogueOption;
		selection->text.assign(option.attribute("option").as_string());
		selection->nextNode = option.attribute("nextNode").as_int();
		selection->activeID = option.attribute("activeID").as_int();
		selection->completeID = option.attribute("completeID").as_int();
		answers->dialogueOptions.push_back(selection);
		answers->answersList.Add((option.attribute("option").as_string()));
	}

	return true;
}