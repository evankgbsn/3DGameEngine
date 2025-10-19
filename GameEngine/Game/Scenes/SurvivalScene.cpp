#include "SurvivalScene.h"

#include "../GameObjects/SurvivalCharacter.h"
#include "../GameObjects/SurvivalSun.h"
#include "../GameObjects/SurvivalTerrain.h"
#include "../GameObjects/SurvivalTree.h"
#include "../GameObjects/SurvivalWater.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "../GameObjects/SurvivalServerFreeCamera.h"


SurvivalScene::SurvivalScene() :
	player(new SurvivalCharacter()),
	sun(new SurvivalSun()),
	terrain(new SurvivalTerrain()),
	tree(new SurvivalTree()),
	water(new SurvivalWater()),
	serverFreeCam(nullptr)
{
	RegisterGameObject(sun, "Sun");
	RegisterGameObject(terrain, "Terrain");
	RegisterGameObject(tree, "Tree");
	RegisterGameObject(water, "Water");
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

	if (tree != nullptr)
	{
		delete tree;
	}

	if (water != nullptr)
	{
		delete water;
	}

	if (serverFreeCam != nullptr)
	{
		delete serverFreeCam;
	}
}

void SurvivalScene::Initialize()
{
	if (SceneManager::SceneLoaded("SurvivalLoginScene"))
	{
		SceneManager::EndScene("SurvivalLoginScene");
		SceneManager::TerminateScene("SurvivalLoginScene");
		SceneManager::UnloadScene("SurvivalLoginScene");
		
	}

	if (!NetworkManager::IsServer())
	{
		static std::function<void(NetworkObject*)> callback = [](NetworkObject* spawnedObject)
			{
				NetworkManager::SyncClientWithServer();
			};

		NetworkManager::Spawn("SurvivalCharacter", &callback);
	}
	else
	{
		serverFreeCam = new SurvivalServerFreeCamera();
		RegisterGameObject(serverFreeCam, "SurvivalServerFreeCamera");
	}

	//RegisterGameObject(player, "Player");

	Scene::Initialize();
}

void SurvivalScene::Terminate()
{
	Scene::Terminate();
}
