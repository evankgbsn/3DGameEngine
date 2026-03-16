#include "Test.h"

#include "../GameObjects/PlaneObj.h"
#include "../GameObjects/FPSPlayer.h"
#include "../GameObjects/Sun.h"
#include "../GameObjects/Crate.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "../GameObjects/ServerFreeCamera.h"
#include "../GameObjects/AK12Bullet.h"
#include "../GameObjects/ShippingContainer.h"
#include "../GameObjects/PointLightObject.h"

Test::Test() :
	player(new FPSPlayer()),
	sun(new Sun()),
	crate(new Crate()),
	serverFreeCam(nullptr),
	localPlayer(nullptr),
	bullet(new AK12Bullet()),
	plane(new PlaneObj()),
	shippingContainer(new ShippingContainer()),
	pointLight(new PointLightObject())
{
	RegisterGameObject(sun, "Sun");
	RegisterGameObject(crate, "Crate");
	RegisterGameObject(plane, "Plane");
	RegisterGameObject(shippingContainer, "ShippingContainer");
	RegisterGameObject(pointLight, "PointLight");
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

	if (plane != nullptr)
	{
		delete plane;
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

void Test::Start()
{
	Scene::Start();

	if (!NetworkManager::IsServer() && localPlayer == nullptr)
	{
		static std::function<void(NetworkObject*)> callback = [this](NetworkObject* spawnedObject)
			{
				localPlayer = spawnedObject;
				NetworkManager::SyncClientWithServer();
			};
	
		NetworkManager::Spawn("FPSPlayer", &callback);
	}
	else if(NetworkManager::IsServer() && serverFreeCam == nullptr)
	{
		serverFreeCam = new ServerFreeCamera();
		RegisterGameObject(serverFreeCam, "ServerFreeCamera");
	}
}

void Test::End()
{
	Scene::End();
}
