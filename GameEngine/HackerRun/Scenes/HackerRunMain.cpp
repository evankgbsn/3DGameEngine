#include "HackerRunMain.h"

#include "GameEngine/Renderer/Renderer.h"

#include "../GameObjects/HackerRunner.h"
#include "../GameObjects/Corruption.h"

HackerRunMain::HackerRunMain() :
	player(new HackerRunner()),
	corruption(new Corruption())
{
	RegisterGameObject(player, "Player");
	RegisterGameObject(corruption, "Corruption");
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
