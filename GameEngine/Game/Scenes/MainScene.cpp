#include "MainScene.h"

#include "../GameObjects/LargePlane.h"
#include "../GameObjects/LightArray.h"
#include "../GameObjects/Character.h"
#include "../GameObjects/Tree.h"
#include "../GameObjects/DirectionalLightGameObject.h"
#include "../GameObjects/Boxes.h"
#include "../GameObjects/FreeCamera.h"
#include "GameEngine/Input/InputManager.h"

MainScene::MainScene() :
	spotLights(new LightArray()),
	directionalLight(new DirectionalLightGameObject()),
	character(new Character()),
	////tree(new Tree()),
	groundPlane(new LargePlane())
	//cam(new FreeCamera())
{

	loadSub = new std::function<void(int)>([](int keyCode)
		{
			SceneManager::LoadScene("Sub");
		});

	unloadSub = new std::function<void(int)>([](int keyCode)
		{
			SceneManager::UnloadScene("Sub");
		});
}

MainScene::~MainScene()
{
	//delete unloadSub;
	//delete loadSub;
	//delete cam;
	delete spotLights;
	delete directionalLight;
	delete character;
	////delete tree;
	delete groundPlane;
}

void MainScene::Initialize()
{
	if (SceneManager::SceneLoaded("Sub"))
	{
		SceneManager::UnloadScene("Sub");
	}

	//RegisterGameObject(cam, "FreeCam");
	RegisterGameObject(directionalLight, "DirectionalLight");
	RegisterGameObject(groundPlane, "Ground");
	RegisterGameObject(character, "Character");
	//RegisterGameObject(tree, "Tree");
	RegisterGameObject(spotLights, "Lights");

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_L, loadSub, "LoadSubscene");
	//InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_U, unloadSub, "UnloadSubscene");
}

void MainScene::Terminate()
{
	//InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_L, "LoadSubscene");
	//InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_U, "UnloadSubscene");

	DeregisterGameObject("Lights");
	//DeregisterGameObject("Tree");
	//DeregisterGameObject("FreeCam");
	DeregisterGameObject("DirectionalLight");
	DeregisterGameObject("Ground");
	DeregisterGameObject("Character");
}