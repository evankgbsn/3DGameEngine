#include "AsteroidField.h"

#include "../GameObjects/Asteroid.h"
#include "../GameObjects/ServerFreeCamera.h"
#include "../GameObjects/PlayerShip.h"
#include "../GameObjects/MainDirectionalLight.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"

AsteroidField::AsteroidField() :
	asteroid(new Asteroid()),
	player(new PlayerShip()),
	mainLight(new MainDirectionalLight()),
	serverFreeCam(nullptr)
{
	RegisterGameObject(asteroid, "Asteroid");
	RegisterGameObject(mainLight, "MainLight");
}

AsteroidField::~AsteroidField()
{
	if (asteroid != nullptr)
	{
		delete asteroid;
	}

	if (serverFreeCam != nullptr)
	{
		delete serverFreeCam;
	}

	if (player != nullptr)
	{
		delete player;
	}

	if (mainLight != nullptr)
	{
		delete mainLight;
	}
}

void AsteroidField::Initialize()
{
	if (SceneManager::SceneLoaded("MainMenu"))
	{
		SceneManager::EndScene("MainMenu");
		SceneManager::TerminateScene("MainMenu");
		SceneManager::UnloadScene("MainMenu");
	}

	Scene::Initialize();
}

void AsteroidField::Terminate()
{
	Scene::Terminate();
}

void AsteroidField::Load()
{
	Scene::Load();
}

void AsteroidField::Unload()
{
	Scene::Unload();
}

void AsteroidField::Start()
{
	Scene::Start();

	if (!NetworkManager::IsServer() && localPlayer == nullptr)
	{
		static std::function<void(NetworkObject*)> callback = [this](NetworkObject* spawnedObject)
			{
				localPlayer = spawnedObject;
				NetworkManager::SyncClientWithServer();
			};

		NetworkManager::Spawn("PlayerShip", &callback);
	}
	else if (NetworkManager::IsServer() && serverFreeCam == nullptr)
	{
		serverFreeCam = new ServerFreeCamera();
		RegisterGameObject(serverFreeCam, "ServerFreeCamera");
	}
}

void AsteroidField::End()
{
	Scene::End();
}
