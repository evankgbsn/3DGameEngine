#include "HackerRunMain.h"

#include "GameEngine/Renderer/Renderer.h"

#include "../GameObjects/HackerRunner.h"
#include "../GameObjects/Corruption.h"
#include "../GameObjects/Spawner.h"
#include "../GameObjects/MemoryBlock.h"

HackerRunMain::HackerRunMain() :
	player(new HackerRunner()),
	corruption(new Corruption()),
	spawner(new Spawner()),
	memBlock(new MemoryBlock())
{
	RegisterGameObject(player, "Player");
	RegisterGameObject(spawner, "Spawner");
}

HackerRunMain::~HackerRunMain()
{
	delete player;
	delete spawner;
	delete memBlock;
}

void HackerRunMain::Initialize()
{
	Scene::Initialize();
}

void HackerRunMain::Terminate()
{
	Scene::Terminate();
}

void HackerRunMain::Load()
{
	Scene::Load();
}

void HackerRunMain::Unload()
{
	Scene::Unload();
}

void HackerRunMain::Start()
{
	Scene::Start();

	Renderer::SetClearColor({ 0.0f,0.0f,0.0f });
}

void HackerRunMain::End()
{
	Scene::End();
}
