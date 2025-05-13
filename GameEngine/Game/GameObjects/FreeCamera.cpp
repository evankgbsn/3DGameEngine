#include "FreeCamera.h"

#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Renderer/Window/WindowManager.h"

FreeCamera::FreeCamera() : 
	GameObject("FreeCamera"),
	wPress(nullptr),
	aPress(nullptr),
	sPress(nullptr),
	dPress(nullptr),
	qPress(nullptr),
	ePress(nullptr),
	cPress(nullptr),
	zPress(nullptr),
	ctrPress(nullptr),
	spacePress(nullptr),
	escPress(nullptr)
{
	CreateInputFunctions();
	RegisterGameObjectClassType<FreeCamera>(this);
}

FreeCamera::~FreeCamera()
{

	delete wPress;
	delete aPress;
	delete sPress;
	delete dPress;
	delete qPress;
	delete ePress;
	delete cPress;
	delete zPress;
	delete ctrPress;
	delete spacePress;
	delete escPress;
}

void FreeCamera::Initialize()
{
	InputRegistration();

	cam = new CameraComponent("FreeCamera");
	AddComponent(cam, "Camera");
}

void FreeCamera::Terminate()
{
	delete cam;

	InputDeregistration();
}

void FreeCamera::GameUpdate()
{
	InputManager::WhenCursorMoved(*whenCursorMove);
}

void FreeCamera::EditorUpdate()
{
}

void FreeCamera::InputRegistration()
{
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, aPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, sPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, dPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Q, qPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_E, ePress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Z, zPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_C, cPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_LEFT_CTRL, ctrPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_SPACE, spacePress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_ESCAPE, escPress, "hiddenCursor");
}

void FreeCamera::InputDeregistration()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_Q, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_E, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_Z, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_C, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_LEFT_CTRL, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_SPACE, "camMovement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_ESCAPE, "hiddenCursor");
}

void FreeCamera::CreateInputFunctions()
{
	float cameraSpeed = 5.0f;
	wPress = new std::function<void(int)>([this, cameraSpeed](int keycode)
		{
			cam->Translate(cam->GetForwardVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	aPress = new std::function<void(int)>([this, cameraSpeed](int keycode)
		{
			cam->Translate(cam->GetRightVector() * -cameraSpeed * TimeManager::DeltaTime());
		});
	sPress = new std::function<void(int)>([this, cameraSpeed](int keycode)
		{
			cam->Translate(cam->GetForwardVector() * -cameraSpeed * TimeManager::DeltaTime());
		});

	dPress = new std::function<void(int)>([this, cameraSpeed](int keycode)
		{
			cam->Translate(cam->GetRightVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	float rotSpeed = 0.001f;
	qPress = new std::function<void(int)>([this, rotSpeed](int keycode)
		{
			cam->Rotate(cam->GetRightVector(), rotSpeed);
		});
	ePress = new std::function<void(int)>([this, rotSpeed](int keycode)
		{
			cam->Rotate(cam->GetRightVector(), -rotSpeed);
		});

	zPress = new std::function<void(int)>([this, rotSpeed](int keycode)
		{
			cam->Rotate(cam->GetUpVector(), rotSpeed);
		});

	cPress = new std::function<void(int)>([this, rotSpeed](int keycode)
		{
			cam->Rotate(cam->GetUpVector(), -rotSpeed);
		});

	ctrPress = new std::function<void(int)>([this, cameraSpeed](int keycode)
		{
			cam->Translate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSpeed * TimeManager::DeltaTime());
		});

	spacePress = new std::function<void(int)>([this, cameraSpeed](int keycode)
		{
			cam->Translate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed * TimeManager::DeltaTime());
		});

	escPress = new std::function<void(int)>([](int keycode)
		{
			static bool toggle = false;
			if (toggle)
			{
				WindowManager::GetWindow("Engine")->DisableCursor();
			}
			else
			{
				WindowManager::GetWindow("Engine")->EnableCursor();
			}

			toggle = !toggle;
		});

	whenCursorMove = new std::function<void(const glm::vec2&)>([this](const glm::vec2& cursorPos)
		{
			static glm::vec2 prevPos;

			float xspeed = 0.0005f;
			float yspeed = 0.0005f;
			if (cursorPos.x != prevPos.x)
			{
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * (float)-(cursorPos.x - prevPos.x));
			}
			if (cursorPos.y != prevPos.y)
			{
				cam->Rotate(cam->GetRightVector(), yspeed * (float)-(cursorPos.y - prevPos.y));
			}

			prevPos = cursorPos;
		});
}

const std::vector<char> FreeCamera::Serialize() const
{
	return std::vector<char>();
}

void FreeCamera::Deserialize(const std::vector<char>& data)
{
}

void FreeCamera::Start()
{
	InputManager::DisableCursor("Engine");
	cam->SetActive();
}

void FreeCamera::End()
{
	InputManager::EnableCursor("Engine");
}

void FreeCamera::Load()
{
}

void FreeCamera::Unload()
{
}
