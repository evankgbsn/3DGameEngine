#include "Editor.h"

#include "../Utils/SingletonHelpers.h"
#include "../Input/InputManager.h"
#include "../Renderer/Camera/CameraManager.h"
#include "../Time/TimeManager.h"
#include "../Renderer/Window/WindowManager.h"

Editor* Editor::instance = nullptr;

Editor::Editor() :
	enabled(true)
{
	CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, "Editor", WindowManager::GetWindow("Engine"));
	SetupEditorInput();
}

Editor::~Editor()
{

}

void Editor::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "Editor");
}

void Editor::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "Editor");
}

void Editor::Update()
{
}

bool Editor::Enabled()
{
	if (instance != nullptr)
	{
		return instance->enabled;
	}

	return false;
}

void Editor::Disbale()
{
	if (instance != nullptr)
	{
		instance->enabled = false;
		CameraManager::SetActiveCamera("Main");
		Logger::Log("Disabled Editor", Logger::Category::Info);
	}
	else
	{
		Logger::Log("Calling Editor::Disbale before Editor::Initialize()", Logger::Category::Error);
	}
}

void Editor::Enable()
{
	if (instance != nullptr)
	{
		instance->enabled = true;
		CameraManager::SetActiveCamera("Editor");
		Logger::Log("Enabled Editor", Logger::Category::Info);
	}
	else
	{
		Logger::Log("Calling Editor::Enable before Editor::Initialize()", Logger::Category::Error);
	}
}

void Editor::SetupEditorInput()
{
	float cameraSpeed = 5.0f;
	static std::function<void(int)> wPress = std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	static std::function<void(int)> aPress = std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * -cameraSpeed * TimeManager::DeltaTime());
		});
	static std::function<void(int)> sPress = std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * -cameraSpeed * TimeManager::DeltaTime());
		});

	static std::function<void(int)> dPress = std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	float rotSpeed = 0.001f;
	static std::function<void(int)> qPress = std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetRightVector(), rotSpeed);
		});
	static std::function<void(int)> ePress = std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetRightVector(), -rotSpeed);
		});

	static std::function<void(int)> zPress = std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetUpVector(), rotSpeed);
		});

	static std::function<void(int)> cPress = std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetUpVector(), -rotSpeed);
		});

	static std::function<void(int)> ctrPress = std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSpeed * TimeManager::DeltaTime());
		});
	
	static std::function<void(int)> spacePress = std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed * TimeManager::DeltaTime());
		});

	static std::function<void(int)> escPress = std::function<void(int)>([](int keycode)
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

	
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_W, &wPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_A, &aPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_S, &sPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_D, &dPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_Q, &qPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_E, &ePress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_Z, &zPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_C, &cPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_LEFT_CTRL, &ctrPress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_SPACE, &spacePress, "camMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_ESCAPE, &escPress, "hiddenCursor");

}
