#include "JoinScene.h"

#include "../GameObjects/JoinUI.h"

JoinScene::JoinScene() :
	ui(new JoinUI())
{
}

JoinScene::~JoinScene()
{
	
	delete ui;
}

void JoinScene::Initialize()
{
	RegisterGameObject(ui, "UI");

	Scene::Initialize();
}

void JoinScene::Terminate()
{

	Scene::Terminate();
}

void JoinScene::Load()
{
	Scene::Load();
}

void JoinScene::Unload()
{
	Scene::Unload();
}
