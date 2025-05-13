#include "FPSTest.h"

#include "../GameObjects/LargePlane.h"
#include "../GameObjects/DirectionalLightGameObject.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/RigidBox.h"

FPSTest::FPSTest() :
	terrain(new LargePlane()),
	light(new DirectionalLightGameObject()),
	player(new Player()),
	box(new RigidBox())
{
}

FPSTest::~FPSTest()
{
	delete player;
	delete terrain;
	delete light;
}

void FPSTest::Initialize()
{
	RegisterGameObject(terrain, "TerrainObject");
	RegisterGameObject(light, "DirectionalLight");
	RegisterGameObject(player, "Player");

	RegisterGameObject(box, "RigidBox");

	for (unsigned int i = 0; i < 10; i++)
	{
		RigidBox* newBox = new RigidBox();
		RegisterGameObject(newBox, std::string("RigidBox") + std::to_string(i));
	}
}

void FPSTest::Terminate()
{
	DeregisterGameObject("Player");
	DeregisterGameObject("TerrainObject");
	DeregisterGameObject("DirectionalLight");
}
