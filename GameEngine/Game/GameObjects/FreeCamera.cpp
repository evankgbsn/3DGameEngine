#include "FreeCamera.h"

#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Renderer/Camera/Camera.h"
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
	WindowManager::GetWindow("Engine")->DisableCursor();
	InputRegistration();

	CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, "Main", WindowManager::GetWindow("Engine"));

	Camera& cam = CameraManager::GetCamera("Main");

	CameraManager::SetActiveCamera("Main");
}

void FreeCamera::Terminate()
{
	CameraManager::DestroyCamera("Main");

	InputDeregistration();
}

void FreeCamera::GameUpdate()
{
	Window* window = WindowManager::GetWindow("Engine");
	glm::vec2 cursorPos = window->GetCursorPosition();
	static glm::vec2 prevPos;

	Camera& cam = CameraManager::GetActiveCamera();

	float xspeed = 0.0005f;
	float yspeed = 0.0005f;
	if (cursorPos.x != prevPos.x)
	{
		cam.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * (float)-(cursorPos.x - prevPos.x));
	}
	if (cursorPos.y != prevPos.y)
	{
		cam.Rotate(cam.GetRightVector(), yspeed * (float)-(cursorPos.y - prevPos.y));
	}

	prevPos = cursorPos;
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
	wPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	aPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * -cameraSpeed * TimeManager::DeltaTime());
		});
	sPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * -cameraSpeed * TimeManager::DeltaTime());
		});

	dPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	float rotSpeed = 0.001f;
	qPress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetRightVector(), rotSpeed);
		});
	ePress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetRightVector(), -rotSpeed);
		});

	zPress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetUpVector(), rotSpeed);
		});

	cPress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetUpVector(), -rotSpeed);
		});

	ctrPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSpeed * TimeManager::DeltaTime());
		});

	spacePress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed * TimeManager::DeltaTime());
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
}

const std::vector<char> FreeCamera::Serialize() const
{
	return std::vector<char>();
}

void FreeCamera::Deserialize(const std::vector<char>& data)
{
}

void FreeCamera::Load()
{
}

void FreeCamera::Unload()
{
}
