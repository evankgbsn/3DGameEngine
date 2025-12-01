#include "Test.h"

#include "../GameObjects/PlaneObj.h"
#include "../GameObjects/FPSPlayer.h"
#include "../GameObjects/Sun.h"
#include "../GameObjects/Crate.h"
#include "../GameObjects/Ground.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "../GameObjects/ServerFreeCamera.h"

Test::Test() :
	sun(new Sun()),
	crate(new Crate()),
	ground(new Ground()),
	serverFreeCam(nullptr)
{
	RegisterGameObject(sun, "Sun");
	RegisterGameObject(crate, "Crate");
	RegisterGameObject(ground, "Ground");
}

Test::~Test()
{
	if (serverFreeCam != nullptr)
	{
		delete serverFreeCam;
	}

	if (sun != nullptr)
	{
		delete sun;
	}

	if (crate != nullptr)
	{
		delete crate;
	}

	if (ground != nullptr)
	{
		delete ground;
	}
}

void Test::Initialize()
{
	if (SceneManager::SceneLoaded("JoinScene"))
	{
		SceneManager::EndScene("JoinScene");
		SceneManager::TerminateScene("JoinScene");
		SceneManager::UnloadScene("JoinScene");
	}

	if (!NetworkManager::IsServer())
	{
		static std::function<void(NetworkObject*)> callback = [](NetworkObject* spawnedObject)
			{
				NetworkManager::SyncClientWithServer();
			};

		NetworkManager::Spawn("FPSPlayer", &callback);
	}
	else
	{
		serverFreeCam = new ServerFreeCamera();
		RegisterGameObject(serverFreeCam, "ServerFreeCamera");
	}

	Scene::Initialize();
}

void Test::Terminate()
{
	Scene::Terminate();
}

void Test::Load()
{
	Scene::Load();
}

void Test::Unload()
{
	Scene::Unload();
}
