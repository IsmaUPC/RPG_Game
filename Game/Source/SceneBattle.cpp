#include "SceneBattle.h"
#include "SceneManager.h"
#include "EntityManager.h"

#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"

#include "SceneManager.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

SceneBattle::SceneBattle()
{
    active = true;
    name.Create("sceneBattle");
}

SceneBattle::~SceneBattle()
{
}

bool SceneBattle::Awake()
{
    LOG("Loading SceneLose");
    bool ret = true;
    return ret;
}

bool SceneBattle::Start()
{
    SDL_Texture* btnTextureAtlas = app->guiManager->btnTextureAtlas;
    app->render->camera.x = app->render->camera.y = 0;

    app->sceneManager->SetEnemeyDetected(false);
    texPalyers = app->tex->Load("Assets/Textures/Characters/atlas_players_battle.png");
    texEnemies = app->tex->Load("Assets/Textures/Enemies/enemies_battle.png");

    // Load Animations
    LoadAnimations();

    // Add Enemies and Players
    AddEntities();
    AddPartners();

    enemies = app->entityManager->spawnQueue;

    // Inicialize the stats
    InicializeStats();
    app->entityManager->spawnQueue = enemies;

    int num = enemies.Count();
    turnSort = new Entity[num];

    // Gui Buttons
    AddBattleMenu(btnTextureAtlas);

    // Colors
    green.r = 0; green.g = 187; green.b = 45;
    yellow.r = 229; yellow.g = 190; yellow.b = 1;
    red.r = 203; red.g = 50; red.b = 52;
    blue.r = 37; blue.g = 40; blue.b = 80;
    orange.r = 255; orange.g = 136; orange.b = 18;

    bool ret;
    ret = LoadMagics("magicList.xml");
    return ret;
}

void SceneBattle::LoadAnimations()
{
    for (int i = 0; i < 4; i++)
    {
        Animation* b = new Animation;
        b->loop = true;
        for (int j = 0; j < 6; j++)
        {
            b->PushBack({ 64 * j,92 * i, 64, 92 });
        }
        animationsPlayer.Add(b);
    }
    // Load Animation Bar Turn
    for (int i = 0; i < 4; i++)
    {
        Animation* b = new Animation;
        b->loop = true;
        for (int j = 0; j < 3; j++)
        {
            b->PushBack({ 384,32 * j + (96 * i), 32, 32 });
        }
        spritesBarTurn.Add(b);
    }
    // Load Animations Enemies
    int numSprites = 1;
    for (int i = 0; i < 12; i++)
    {
        Animation* b = new Animation;
        b->loop = true;
        if (i % 2) numSprites = 2;
        else numSprites = 4;
        if (i == 6) numSprites = 5;
        else if (i == 8) numSprites = 5;
        else if (i == 10) numSprites = 6;
        if ((i+1) % 4 == 0 && i != 0)
        {
            for (int j = 0; j < 4; j++)
            {
                b->PushBack({ 32 * j,64 * i, 32, 32 });
            }
            spritesBarTurn.Add(b);
        }
        else
        {
            for (int j = 0; j < numSprites; j++)
            {
                b->PushBack({ 128 * j,64 * i, 128, 64 });
            }
            animationsEnemies.Add(b);
        }
    }
}

void SceneBattle::AddEntities()
{
    int id = app->entityManager->GetCurrentEntity()->entityData.id;
    int level = app->entityManager->GetCurrentEntity()->entityData.level;
    int randomLvl = 1;
    switch (id)
    {
    case 1:
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 14, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 11, 15, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 11, 19, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 8, 17, 0, randomLvl);
        break;

    case 2:
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(FIGHTER, 14, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 11, 15, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(SAPLING, 11, 19, 0, randomLvl);
        break;

    case 3:
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(SAPLING, 14, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(FIGHTER, 11, 15, 0, randomLvl);
        break;
    default:
        break;
    }
}
void SceneBattle::AddPartners()
{
    // Partners and Player
    int num = app->player->GetNumPartners();
    app->entityManager->AddEntity(KENZIE_, 26, 13, 0, app->player->playerData.level);

    for (int i = 0; i < num; i++)
    {
        switch (app->player->GetPartners()[i].type)
        {
        case KEILER:
            app->entityManager->AddEntity(KEILER_, 29, 15, 0, app->player->GetPartners()[i].level);
            break;
        case ISRRA:
            app->entityManager->AddEntity(ISRRA_, 29, 19, 0, app->player->GetPartners()[i].level);
            break;
        case BRENDA:
            app->entityManager->AddEntity(BRENDA_, 26, 17, 0, app->player->GetPartners()[i].level);
            break; 
        default:
            break;
        }
    }
}
void SceneBattle::InicializeStats()
{
    float strong = 1;
    for (int i = 0; i < enemies.Count(); i++)
    {
        int level = enemies.At(i)->data->entityData.level;
        // Add switch for assign strong, if is a type enemy strong = x, if is other type strong = y
        TypeEntity pType = enemies.At(i)->data->entityData.type;
        if (pType == KENZIE_ || pType == KEILER_ || pType == ISRRA_ || pType == BRENDA_)
        {
            switch (pType)
            {
            case KENZIE_ :
                enemies.At(i)->data->stats.attack = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.defense = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.health = 2 * level + 6;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 3 * level +8;
                enemies.At(i)->data->stats.speed = 2.5 * level + 7.5;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(0)->data;
                break;
            case KEILER_:
                enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.defense = 1.5 * level + 5.5;
                enemies.At(i)->data->stats.health = 4 * level + 10;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 2.5 * level + 7.5;
                enemies.At(i)->data->stats.speed = 2.5 * level + 8.5;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(1)->data;
                break;
            case ISRRA_:
                enemies.At(i)->data->stats.attack = 2 * level + 7;
                enemies.At(i)->data->stats.defense = 2 * level + 7;
                enemies.At(i)->data->stats.health = 2.5 * level + 7.5;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 3.5 * level + 8.5;
                enemies.At(i)->data->stats.speed = 2.5 * level + 7.5;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(2)->data;
                break;
            case BRENDA_:
                enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.defense = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.health = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.speed = 1.5 * level + 6.5;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(3)->data;
                break;

            default:
                break;
            }
            enemies.At(i)->data->entityData.texture = texPalyers;
        }
        else
        {
            switch (pType)
            {
            case BANDIT:
                strong = 1;
                enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(0)->data;
                break;
            case FIGHTER:
                strong = 1.1;
                enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(3)->data;
                break;
            case SAPLING:
                strong = 1.2;
                enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(6)->data;
                break;
            default:
                break;
            }
            enemies.At(i)->data->stats.attack = ((2 * level + 7) * 0.75) * strong;
            enemies.At(i)->data->stats.defense = ((2 * level + 7) * 0.8) * strong;
            enemies.At(i)->data->stats.health = ((2.5 * level + 7.5) * 1.5) * strong;
            enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
            enemies.At(i)->data->stats.mana = (enemies.At(i)->data->stats.health / 2) * strong;
            enemies.At(i)->data->stats.speed = ((2.5 * level + 7.5) * 0.9) * strong;
            enemies.At(i)->data->stats.exp = (sqrt(CalculateExp(level)) / 3) * strong;

            enemies.At(i)->data->entityData.texture = texEnemies;
        }
    }
}
void SceneBattle::AddBattleMenu(SDL_Texture* btnTextureAtlas)
{
    int padding = 90;
    int yPosition = 20;
    btnAttack = new GuiButton(20, { WINDOW_W - 200 ,yPosition + (padding * 0),  183, 91 }, "ATTACK", RECTANGLE, btnTextureAtlas);
    btnAttack->SetObserver(this);
    app->guiManager->AddGuiButton(btnAttack);

    btnMagic = new GuiButton(21, { WINDOW_W - 200 , yPosition + (padding * 1),  183, 91 }, "MAGIC", RECTANGLE, btnTextureAtlas);
    btnMagic->SetObserver(this);
    app->guiManager->AddGuiButton(btnMagic);

    btnDefense = new GuiButton(22, { WINDOW_W - 200,yPosition + (padding * 2),  183, 91 }, "DEFENSE", RECTANGLE, btnTextureAtlas);
    btnDefense->SetObserver(this);
    app->guiManager->AddGuiButton(btnDefense);

    btnExit = new GuiButton(23, { WINDOW_W - 200, yPosition + (padding * 3),  183, 91 }, "EXIT", RECTANGLE, btnTextureAtlas);
    btnExit->SetObserver(this);
    app->guiManager->AddGuiButton(btnExit);
}

bool SceneBattle::PreUpdate()
{
    return true;
}

bool SceneBattle::Update(float dt_)
{
    //for (int i = 0; i < enemies.Count(); i++)
    //{
    //    enemies.At(i)->data->stats.health -= dt*2;
    //}
    dt = dt_;
    SpeedAnimationCheck(dt_);

    return true;
}
bool SceneBattle::PostUpdate()
{
    if (!assigneDone)AssignEntities();

    app->render->DrawTexture(img, 0, 0);

    // Draw Bar lives
    for (int i = 0; i < enemies.Count(); i++)
    {
        int posX = (int)enemies.At(i)->data->entityData.position.x + 2*enemies.At(i)->data->entityData.pointsCollision[0].x + 2*enemies.At(i)->data->entityData.centerPoint.x;
        int posY = (int)enemies.At(i)->data->entityData.position.y - 30;
        rec = { posX-40, posY, 80, 16};
        live = rec;
        live.w = enemies.At(i)->data->stats.health * rec.w / enemies.At(i)->data->stats.maxHealth;

        DrawBarLives();
    }
    for (int i = 0; i < partners.Count(); i++)
    {
        int posX = (int)partners.At(i)->data->entityData.position.x + partners.At(i)->data->entityData.centerPoint.x;
        int posY = (int)partners.At(i)->data->entityData.position.y - 30;
        rec = { posX - 40, posY, 80, 16 };
        live = rec;
        live.w = partners.At(i)->data->stats.health * rec.w / partners.At(i)->data->stats.maxHealth;

        DrawBarLives();
    }
    // Draw turn bar
    app->render->DrawRectangle({20, 30, 48, 64*tam-16}, blue.r, blue.g, blue.b, 100);
    DrawTurnBar();
    app->render->DrawRectangle({ 20, 30, 48, 64 * tam - 16 }, orange.r, orange.g, orange.b, 255, false);
    return true;
}

void SceneBattle::DrawBarLives()
{
    if (live.w > rec.w) live.w = rec.w;
    if (live.w > rec.w / 2) app->render->DrawRectangle(live, green.r, green.g, green.b);
    if (live.w < rec.w / 2) app->render->DrawRectangle(live, yellow.r, yellow.g, yellow.b);
    if (live.w < rec.w / 4) app->render->DrawRectangle(live, red.r, red.g, red.b);

    app->render->DrawRectangle(rec, 71, 75, 78, 255, false);
}

void SceneBattle::DrawTurnBar()
{
    if (moveBarTurn) offset -= dt * 30, numArrows = 0;
    else offset = 38, numArrows = 1;
    for (int i = 0; i < tam; i++)
    {
        switch (turnSort[i].entityData.type)
        {
        case KENZIE_:
            if (i == 0) {
                spritesBarTurn.At(0)->data->Update();
                face = spritesBarTurn.At(0)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(0)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0) 
            {
                face = spritesBarTurn.At(0)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case KEILER_:
            if (i == 0) {
                spritesBarTurn.At(1)->data->Update();
                face = spritesBarTurn.At(1)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(1)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(1)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case ISRRA_:
            if (i == 0) {
                spritesBarTurn.At(2)->data->Update();
                face = spritesBarTurn.At(2)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(2)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(2)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case BRENDA_:
            if (i == 0) {
                spritesBarTurn.At(3)->data->Update();
                face = spritesBarTurn.At(3)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(3)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(3)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;

        case BANDIT:
            if (i == 0) {
                spritesBarTurn.At(4)->data->Update();
                face = spritesBarTurn.At(4)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(4)->data->frames[1];
            app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(4)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case FIGHTER:
            if (i == 0) {
                spritesBarTurn.At(5)->data->Update();
                face = spritesBarTurn.At(5)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(5)->data->frames[1];
            app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(5)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case SAPLING:
            if (i == 0) {
                spritesBarTurn.At(6)->data->Update();
                face = spritesBarTurn.At(6)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(6)->data->frames[1];
            app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(6)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        default:
            break;
        }
        face = { 416,0, 32,14 };
        if (i < tam - numArrows)app->render->DrawTexture(texPalyers, 28, 64 * i + 40 + offset, &face);
    }
    if (moveBarTurn)
    {
        face = {20,0,48,30};
        app->render->DrawTexture(img, 20, 0, &face);
        face = { 20,64 * tam - 16 +30,48,64 };
        app->render->DrawTexture(img, 20, 64 * tam - 16+30, &face);
    }
    if (offset < -64 + 38)moveBarTurn = false, DisplaceToLeft();
}

bool SceneBattle::CleanUp()
{
    bool ret = true;
    app->tex->UnLoad(img);
    app->tex->UnLoad(texPalyers);
    app->tex->UnLoad(texEnemies);

    spritesBarTurn.Clear();
    animationsPlayer.Clear();
    animationsEnemies.Clear();

    delete[] turnSort;

    magics.Clear();
    app->entityManager->ClearList(ret);
    enemies = app->entityManager->entities;
    partners = app->entityManager->partners;


    return ret;
}
void SceneBattle::SpeedAnimationCheck(float dt)
{
    for (int i = 0; i < spritesBarTurn.Count(); i++)
    {
        spritesBarTurn.At(i)->data->speed = dt * 6;
    }
    for (int i = 0; i < animationsPlayer.Count(); i++)
    {
        animationsPlayer.At(i)->data->speed = dt * 6;
    }
    for (int i = 0; i < animationsEnemies.Count(); i++)
    {
        animationsEnemies.At(i)->data->speed = dt * 2;
    }
}

bool SceneBattle::LoadMagics(const char* file)
{
    pugi::xml_parse_result result = magicDoc.load_file(file);

    if (result == NULL)
    {
        LOG("Could not load the file %s. pugi error: %s", file, result.description());
        return false;
    }
    else
    {
        pugi::xml_node character = magicDoc.first_child().child("character");

        for (character; character != NULL; character = character.next_sibling("character"))
        {
            for (pugi::xml_node n = character.child("magic"); n != NULL; n = n.next_sibling("magic"))
            {
                Magic* magic = new Magic;
                magic->id = character.attribute("ID").as_int();
                magic->level = n.attribute("level").as_int();
                magic->name = n.attribute("name").as_string("");
                magic->damage = n.attribute("damage").as_int();
                magic->mana = n.attribute("mana").as_int();
                magic->description = n.attribute("description").as_string("");
                magics.Add(magic);
            }
        }
    }
    return true;
}

void SceneBattle::AssignEntities()
{
    enemies = app->entityManager->entities;
    partners = app->entityManager->partners;

    for (int i = 0; i < enemies.Count(); i++)
    {
        turnSort[i].entityData = enemies.At(i)->data->entityData;
        turnSort[i].stats = enemies.At(i)->data->stats;
    }
    int j = 0;
    tam = enemies.Count() + partners.Count();
    for (int i = enemies.Count(); i < tam; i++)
    {
        turnSort[i].entityData = partners.At(j)->data->entityData;
        turnSort[i].stats = partners.At(j)->data->stats;
        j++;
    }
    BubbleSort();

    assigneDone = true;
}
bool SceneBattle::OnGuiMouseClickEvent(GuiControl* control)
{
    switch (control->type)
    {
    case GuiControlType::BUTTON:
    {
        //ATTACK
        if (control->id == 20)
        {
            //Next turn
            while (turnSort[turn + 1].isAlive == false)
            {
                turn += 1;
                if (turn >= tam) turn = 0;
            }
            if (turn < tam-1)
            {
                turn++;
            }
            else
            {
                turn = 0;
            }
           // DisplaceToLeft();

        }
        //MAGIC
        else if (control->id == 21)
        {
            for (int i = 0; i < magics.Count(); i++)
            {
                if (magics.At(i)->data->id == turnSort[turn].entityData.type)
                {
                    LOG("%s",magics.At(i)->data->name.GetString());
                }
            }
        }
        //DEFENSE
        else if (control->id == 22)
        {
            switch (turnSort[turn].entityData.type)
            {
            case 15:
                LOG("Kenzie");
                break;
            case 16:
                LOG("Keiler");
                break;
            case 17:
                LOG("Isrra");
                break;
            case 18:
                LOG("Brenda");
                break;
            default:
                break;
            }
        }
        //EXIT
        else if (control->id == 23)
        {
            isContinue = true;
            TransitionToScene(SceneType::LEVEL1);
        }
    }
    default: break;
    }
    return true;
}

int SceneBattle::CalculateExp(int level)
{
    return (0.04 * (level * level * level) + 0.8 * (level * level) + 2 * level) * 3.5;
}
void SceneBattle::BubbleSort()
{
    int numSwaps = -1;
    while (numSwaps != 0)
    {
        numSwaps = 0;
        for (int i = 0; i < tam - 1; i++)
        {
            Entity aux;
            if (turnSort[i].stats.speed < turnSort[i + 1].stats.speed)
            {
                aux = turnSort[i];
                turnSort[i] = turnSort[i + 1];
                turnSort[i + 1] = aux;
                numSwaps++;
            }
        }
    }
}
void SceneBattle::DisplaceToLeft()
{
    Entity aux = turnSort[0];
    for (int i = 0; i < tam-1; i++)
    {
        turnSort[i] = turnSort[i + 1];
    }
    turnSort[tam - 1] = aux;
}
