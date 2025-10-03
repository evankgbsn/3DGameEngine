#include "SurvivalScene.h"

#include "../GameObjects/SurvivalCharacter.h"
#include "../GameObjects/SurvivalSun.h"
#include "../GameObjects/SurvivalTerrain.h"
#include "../GameObjects/SurvivalTree.h"
#include "../GameObjects/SurvivalWater.h"


SurvivalScene::SurvivalScene() :
	player(new SurvivalCharacter()),
	sun(new SurvivalSun()),
	terrain(new SurvivalTerrain()),
	tree(new SurvivalTree()),
	water(new SurvivalWater())
{

	deserialize = new std::function<void(int)>([this](int keyCode)
		{
			this->Deserialize("Assets/Scenes/SurvivalScene.xml");
		});

}

SurvivalScene::~SurvivalScene()
{
	if (terrain != nullptr)
	{
		delete terrain;
	}

	if (sun != nullptr)
	{
		delete sun;
	}

	if (player != nullptr)
	{
		delete player;
	}

	if (deserialize != nullptr)
	{
		delete deserialize;
	}

	if (tree != nullptr)
	{
		delete tree;
	}

	if (water != nullptr)
	{
		delete water;
	}
}

void SurvivalScene::Initialize()
{
	RegisterGameObject(player, "Player");
	RegisterGameObject(sun, "Sun");
	RegisterGameObject(terrain, "Terrain");
	RegisterGameObject(tree, "Tree");
	RegisterGameObject(water, "Water");
}

void SurvivalScene::Terminate()
{
	Scene::Terminate();

	DeregisterGameObject("Water");
	DeregisterGameObject("Player");
	DeregisterGameObject("Sun");
	DeregisterGameObject("Terrain");
	DeregisterGameObject("Tree");
}
