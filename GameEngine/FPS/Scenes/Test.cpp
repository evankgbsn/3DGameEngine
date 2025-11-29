#include "Test.h"

#include "../GameObjects/PlaneObj.h"
#include "../GameObjects/FPSPlayer.h"
#include "../GameObjects/Sun.h"

Test::Test() :
	plane(new PlaneObj()),
	player(new FPSPlayer()),
	sun(new Sun())
{
	RegisterGameObject(plane, "Plane");
	RegisterGameObject(player, "Player");
	RegisterGameObject(sun, "Sun");
}

Test::~Test()
{
	if (plane != nullptr)
	{
		delete plane;
	}

	if (player != nullptr)
	{
		delete player;
	}

	if (sun != nullptr)
	{
		delete sun;
	}
}

void Test::Initialize()
{
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
