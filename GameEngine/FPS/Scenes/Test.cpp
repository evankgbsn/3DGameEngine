#include "Test.h"

#include "../GameObjects/PlaneObj.h"
#include "../GameObjects/FPSPlayer.h"
#include "../GameObjects/Sun.h"
#include "../GameObjects/Crate.h"

Test::Test() :
	plane(new PlaneObj()),
	player(new FPSPlayer()),
	sun(new Sun()),
	crate(new Crate())
{
	RegisterGameObject(plane, "Plane");
	RegisterGameObject(player, "Player");
	RegisterGameObject(sun, "Sun");
	RegisterGameObject(crate, "Crate");
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

	if (crate != nullptr)
	{
		delete crate;
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
