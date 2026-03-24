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
#include "../GameObjects/WarehouseDoorway.h"
#include "../GameObjects/WarehouseBrickWall.h"
#include "../GameObjects/WarehouseWindowWall.h"
#include "../GameObjects/WarehouseHorizontalBeam.h"
#include "../GameObjects/WarehouseVerticalBeam.h"
#include "../GameObjects/WarehouseVerticalBeamCorner.h"
#include "../GameObjects/WarehouseRoofWall.h"
#include "../GameObjects/WarehouseRoofHorizontalBeam.h"
#include "../GameObjects/WarehouseVerticalBeamHalf.h"
#include "../GameObjects/WarehouseRoofPannel.h"

Test::Test() :
	player(new FPSPlayer()),
	sun(new Sun()),
	crate(new Crate()),
	serverFreeCam(nullptr),
	localPlayer(nullptr),
	bullet(new AK12Bullet()),
	plane(new PlaneObj()),
	shippingContainer(new ShippingContainer()),
	pointLight(new PointLightObject()),
	warehouseDoorway(new WarehouseDoorway()),
	warehouseBrickWall(new WarehouseBrickWall()),
	warehouseWindowWall(new WarehouseWindowWall()),
	warehouseHorizontalBeam(new WarehouseHorizontalBeam()),
	warehouseVerticalBeam(new WarehouseVerticalBeam()),
	warehouseVerticalBeamCorner(new WarehouseVerticalBeamCorner()),
	warehouseRoofWall(new WarehouseRoofWall()),
	warehouseRoofHorizontalBeam(new WarehouseRoofHorizontalBeam()),
	warehouseVerticalBeamHalf(new WarehouseVerticalBeamHalf()),
	warehouseRoofPannel(new WarehouseRoofPannel())
{
	RegisterGameObject(sun, "Sun");
	RegisterGameObject(crate, "Crate");
	RegisterGameObject(plane, "Plane");
	RegisterGameObject(shippingContainer, "ShippingContainer");
	RegisterGameObject(pointLight, "PointLight");
	RegisterGameObject(warehouseDoorway, "WarehouseDoorway");
	RegisterGameObject(warehouseBrickWall, "WarehouseBrickWall");
	RegisterGameObject(warehouseWindowWall, "WarehouseWindowWall");
	RegisterGameObject(warehouseHorizontalBeam, "WarehouseHorizontalBeam");
	RegisterGameObject(warehouseVerticalBeam, "WarehouseVerticalBeam");
	RegisterGameObject(warehouseVerticalBeamCorner, "WarehouseVerticalBeamCorner");
	RegisterGameObject(warehouseRoofWall, "WarehouseRoofWall");
	RegisterGameObject(warehouseRoofHorizontalBeam, "WarehouseRoofHorizontalBeam");
	RegisterGameObject(warehouseVerticalBeamHalf, "WarehouseVerticalBeamHalf");
	RegisterGameObject(warehouseRoofPannel, "WarehouseRoofPannel");
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

	if (shippingContainer != nullptr)
	{
		delete shippingContainer;
	}

	if (pointLight != nullptr)
	{
		delete pointLight;
	}

	if (warehouseDoorway != nullptr)
	{
		delete warehouseDoorway;
	}

	if (warehouseWindowWall != nullptr)
	{
		delete warehouseWindowWall;
	}

	if (warehouseHorizontalBeam != nullptr)
	{
		delete warehouseHorizontalBeam;
	}

	if (warehouseVerticalBeam != nullptr)
	{
		delete warehouseVerticalBeam;
	}

	if (warehouseVerticalBeamCorner != nullptr)
	{
		delete warehouseVerticalBeamCorner;
	}

	if (warehouseRoofWall != nullptr)
	{
		delete warehouseRoofWall;
	}

	if (warehouseRoofHorizontalBeam != nullptr)
	{
		delete warehouseRoofHorizontalBeam;
	}

	if (warehouseVerticalBeamHalf != nullptr)
	{
		delete warehouseVerticalBeamHalf;
	}

	if (warehouseRoofPannel != nullptr)
	{
		delete warehouseRoofPannel;
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
