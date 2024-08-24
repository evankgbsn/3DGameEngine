#include "Editor.h"

#include "../Utils/SingletonHelpers.h"
#include "../Input/InputManager.h"
#include "../Renderer/Camera/CameraManager.h"
#include "../Time/TimeManager.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Select/SelectionManager.h"
#include "../GameObject/GameObject.h"

Editor* Editor::instance = nullptr;

Editor::Editor() :
	enabled(true),
	grid(nullptr),
	shiftPressed(false),
	gridSpeed(5.0f)
{
	CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, "Editor", WindowManager::GetWindow("Engine"));
	SetupEditorInput();
	InitializeGrid();

	SelectionManager::Initialize();
}

Editor::~Editor()
{
	SelectionManager::Terminate();
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
	if (instance != nullptr)
	{
		if (!instance->grid->IsDisabled())
		{
			const glm::vec3& cameraPosition = CameraManager::GetActiveCamera().GetPosition();
			int x = cameraPosition.x;
			int z = cameraPosition.z;
			instance->grid->SetTranslation(glm::vec3(x, instance->grid->GetTranslation().y, z));
		}

		SelectionManager::Update();
	}
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

		if (!instance->grid->IsDisabled())
		{
			GraphicsObjectManager::Disable(instance->grid);
		}

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

		if (instance->grid->IsDisabled())
		{
			GraphicsObjectManager::Enable(instance->grid);
		}

		Logger::Log("Enabled Editor", Logger::Category::Info);
	}
	else
	{
		Logger::Log("Calling Editor::Enable before Editor::Initialize()", Logger::Category::Error);
	}
}

bool Editor::ShiftPressed()
{
	if (instance != nullptr)
	{
		return instance->shiftPressed;
	}

	return false;
}

float Editor::GetGridY()
{
	if (instance != nullptr)
	{
		if (instance->grid != nullptr)
		{
			return instance->grid->GetTranslation().y;
		}
	}

	return 0.0f;
}

void Editor::SetGridY(float newY)
{
	if (instance != nullptr)
	{
		if (instance->grid != nullptr)
		{
			const glm::vec3 translation = instance->grid->GetTranslation();
			instance->grid->SetTranslation({ translation.x, newY, translation.z });
		}
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

	static std::function<void(int)> shiftPress = std::function<void(int)>([](int keyCode)
		{
			if (instance != nullptr)
			{
				instance->shiftPressed = true;
			}
		});

	static std::function<void(int)> shiftRelease = std::function<void(int)>([](int keyCode)
		{
			if (instance != nullptr)
			{
				instance->shiftPressed = false;
			}
		});

	auto snapObjectToGrid = []()
		{
			GameObject* selection = SelectionManager::GetSelection();

			if (selection != nullptr)
			{
				const glm::vec3& gridTranslation = instance->grid->GetTranslation();
				const glm::vec3& objectTranslation = selection->GetPosition();

				selection->SetPosition({ objectTranslation.x, gridTranslation.y, objectTranslation.z });
			}
		};

	static std::function<void(int)> upArrowPressed = std::function<void(int)>([snapObjectToGrid](int keyCode)
		{
			if (instance != nullptr)
			{
				if (!instance->grid->IsDisabled())
				{
					if (!instance->shiftPressed)
					{
						instance->grid->Translate(glm::vec3(0.0f, 1.0f, 0.0f) * TimeManager::DeltaTime() * instance->gridSpeed);	
					}
					snapObjectToGrid();
				}
			}
		});

	static std::function<void(int)> downArrowPressed = std::function<void(int)>([snapObjectToGrid](int keyCode)
		{
			if (instance != nullptr)
			{
				if (!instance->grid->IsDisabled())
				{
					if (!instance->shiftPressed)
					{
						instance->grid->Translate(glm::vec3(0.0f, -1.0f, 0.0f) * TimeManager::DeltaTime() * instance->gridSpeed);

					}
					else
					{
					}
					snapObjectToGrid();
				}
			}
		});


	static std::function<void(int)> upArrowPress = std::function<void(int)>([snapObjectToGrid](int keyCode)
		{
			if (instance != nullptr)
			{
				if (!instance->grid->IsDisabled())
				{
					if (instance->shiftPressed)
					{
						glm::vec3 gridPos = instance->grid->GetTranslation();

						int y = gridPos.y;

						y += 1;

						instance->grid->SetTranslation(glm::vec3(gridPos.x, y, gridPos.z));

						snapObjectToGrid();
					}
				}
			}
		});

	static std::function<void(int)> downArrowPress = std::function<void(int)>([snapObjectToGrid](int keyCode)
		{
			if (instance != nullptr)
			{
				if (!instance->grid->IsDisabled())
				{
					if (instance->shiftPressed)
					{
						glm::vec3 gridPos = instance->grid->GetTranslation();

						int y = gridPos.y;

						y -= 1;

						instance->grid->SetTranslation(glm::vec3(gridPos.x, y, gridPos.z));

						snapObjectToGrid();
					}
				}
			}
		});
	
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_UP, &upArrowPress, "gridMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_DOWN, &downArrowPress, "gridMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_SHIFT, &shiftRelease, "gridMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_SHIFT, &shiftPress, "gridMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_UP, &upArrowPressed, "gridMovement");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_DOWN, &downArrowPressed, "gridMovement");
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

void Editor::InitializeGrid()
{
	Model* gridModel = ModelManager::CreateModelTerrain("EditorGrid", "Assets/Texture/planeHeightMap.png", 500.0f, 500.0f, 500U, 500U, 0, 0.0f);

	grid = GraphicsObjectManager::CreateGO3DColored(gridModel, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	grid->SetDrawMode(GO3D::Mode::LINE);
}

void Editor::TerminateGrid()
{
	GraphicsObjectManager::Delete(grid);
}
