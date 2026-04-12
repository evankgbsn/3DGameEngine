#include "HackerRunMain.h"

#include "GameEngine/Renderer/Renderer.h"

#include "../GameObjects/HackerRunner.h"
#include "../GameObjects/Corruption.h"
#include "../GameObjects/Spawner.h"

HackerRunMain::HackerRunMain() :
	player(new HackerRunner()),
	corruption(new Corruption()),
	spawner(new Spawner())
{
	RegisterGameObject(player, "Player");
	RegisterGameObject(spawner, "Spawner");
}

HackerRunMain::~HackerRunMain()
{
	delete player;
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
