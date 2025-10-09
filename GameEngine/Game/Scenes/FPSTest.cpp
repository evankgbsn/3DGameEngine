#include "FPSTest.h"

#include "../GameObjects/LargePlane.h"
#include "../GameObjects/DirectionalLightGameObject.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/RigidBox.h"
#include "GameEngine/Input/InputManager.h"

FPSTest::FPSTest() :
	terrain(new LargePlane()),
	light(new DirectionalLightGameObject()),
	player(new Player()),
	box(new RigidBox())
{
	deserialize = new std::function<void(int)>([this](int keyCode)
		{
			this->Deserialize("Assets/Scenes/FPSTest.xml");
		});
}

FPSTest::~FPSTest()
{
	delete deserialize;
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

	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_L, deserialize, "deserialize");
}

void FPSTest::Terminate()
{
	InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_L, "deserialize");
}
