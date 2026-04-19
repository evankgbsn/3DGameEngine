#include "MainMenu.h"

#include "../GameObjects/NetworkUI.h"

MainMenu::MainMenu() :
	networkUI(new NetworkUI())
{
	RegisterGameObject(networkUI, "NetworkUI");
}

MainMenu::~MainMenu()
{
	delete networkUI;
}

void MainMenu::Initialize()
{
	Scene::Initialize();
}

void MainMenu::Terminate()
{
	Scene::Terminate();
}

void MainMenu::Load()
{
	Scene::Load();
}

void MainMenu::Unload()
{
	Scene::Unload();
}

void MainMenu::Start()
{
	Scene::Start();
}

void MainMenu::End()
{
	Scene::End();
}
