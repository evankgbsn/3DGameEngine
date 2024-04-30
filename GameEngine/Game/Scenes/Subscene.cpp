#include "Subscene.h"

#include "../GameObjects/Boxes.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "../GameObjects/FreeCamera.h"

Subscene::Subscene() :
	boxes(new Boxes()),
	cam(new FreeCamera())
{
	loadMain = new std::function<void(int)>([](int keyCode)
		{
			SceneManager::LoadScene("Main");
		});
}

Subscene::~Subscene()
{
	delete loadMain;
	delete boxes;
}

void Subscene::Initialize()
{

	SceneManager::UnloadScene("Main");

	RegisterGameObject(boxes, "SpinnningBoxes");
	RegisterGameObject(cam, "FreeCam");

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_L, loadMain, "LoadMainScene");
}

void Subscene::Terminate()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_L, "LoadMainScene");

	DeregisterGameObject("FreeCam");
	DeregisterGameObject("SpinningBoxez");
}
