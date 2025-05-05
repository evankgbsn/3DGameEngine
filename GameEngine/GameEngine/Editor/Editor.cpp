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
#include "UI/EditorUI.h"

Editor* Editor::instance = nullptr;

Editor::Editor() :
	enabled(true),
	grid(nullptr),
	shiftPressed(false),
	rightMousePressed(false),
	gridSpeed(5.0f),
	ui(new EditorUI())
{
	Camera& cam = CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, "Editor", WindowManager::GetWindow("Engine"));
	cam.SetPosition(glm::vec3(10.0f, 10.0f, 10.0f));
	cam.SetTarget(glm::vec3(0.0f));
	SetupEditorInput();
	InitializeGrid();
}

Editor::~Editor()
{
	delete ui;
}

void Editor::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "Editor");
	SelectionManager::Initialize();
}

void Editor::Terminate()
{
	SelectionManager::Terminate();
	SingletonHelpers::TerminateSingleton(&instance, "Editor");
}

void Editor::Update()
{
	if (instance != nullptr)
	{
		if (!instance->grid->IsDisabled())
		{
			const glm::vec3& cameraPosition = CameraManager::GetActiveCamera().GetPosition();
			int x = (int)cameraPosition.x;
			int z = (int)cameraPosition.z;
			instance->grid->SetTranslation(glm::vec3(x, instance->grid->GetTranslation().y, z));

			instance->FreeCameraMovement();
		}

		SelectionManager::Update();

		instance->ui->Update();
	}
}

bool Editor::IsEnabled()
{
	if (instance != nullptr)
	{
		return instance->enabled;
	}

	return false;
}

void Editor::Disable()
{
	if (instance != nullptr)
	{
		instance->enabled = false;
		CameraManager::SetActiveCamera("Main");

		if (!instance->grid->IsDisabled())
		{
			GraphicsObjectManager::Disable(instance->grid);
		}

		for (std::function<void()>* func : instance->onEditorDisableCallbacks)
		{
			(*func)();
		}

		if (!instance->ui->IsDisabled())
		{
			instance->ui->Disable();
		}

		SelectionManager::ClearSelection();

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

		for (std::function<void()>* func : instance->onEditorEnableCallbacks)
		{
			(*func)();
		}

		if (instance->ui->IsDisabled())
		{
			instance->ui->Enable();
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

void Editor::RegisterOnEditorEnable(std::function<void()>* function)
{
	if (instance != nullptr)
	{
		instance->onEditorEnableCallbacks.push_back(function);
	}
}

void Editor::DeregisterOnEditorEnable(std::function<void()>* function)
{
	if (instance != nullptr)
	{
		instance->onEditorEnableCallbacks.remove(function);
	}
}

void Editor::RegisterOnEditorDisable(std::function<void()>* function)
{
	if (instance != nullptr)
	{
		instance->onEditorDisableCallbacks.push_back(function);
	}
}

void Editor::DeregisterOnEditorDisable(std::function<void()>* function)
{
	if (instance != nullptr)
	{
		instance->onEditorDisableCallbacks.remove(function);
	}
}

void Editor::SetupEditorInput()
{
	float cameraSpeed = 5.0f;
	static std::function<void(int)> wPress = std::function<void(int)>([cameraSpeed](int keyCode)
		{
			if (instance != nullptr)
			{
				if (instance->rightMousePressed)
				{
					Camera& cam = CameraManager::GetActiveCamera();
					cam.Translate(cam.GetForwardVector() * cameraSpeed * TimeManager::DeltaTime());
				}
			}
		});

	static std::function<void(int)> aPress = std::function<void(int)>([cameraSpeed](int keyCode)
		{
			if (instance != nullptr)
			{
				if (instance->rightMousePressed)
				{
					Camera& cam = CameraManager::GetActiveCamera();
					cam.Translate(-cam.GetRightVector() * cameraSpeed * TimeManager::DeltaTime());
				}
			}
		});

	static std::function<void(int)> sPress = std::function<void(int)>([cameraSpeed](int keyCode)
		{
			if (instance != nullptr)
			{
				if (instance->rightMousePressed)
				{
					Camera& cam = CameraManager::GetActiveCamera();
					cam.Translate(-cam.GetForwardVector() * cameraSpeed * TimeManager::DeltaTime());
				}
			}
		});

	static glm::vec2 lookPreservationCursorPosition;

	static std::function<void(int)> dPress = std::function<void(int)>([cameraSpeed](int keyCode)
		{
			if (instance != nullptr)
			{
				if (instance->rightMousePressed)
				{
					Camera& cam = CameraManager::GetActiveCamera();
					cam.Translate(cam.GetRightVector() * cameraSpeed * TimeManager::DeltaTime());
				}
			}
		});


	static std::function<void(int)> rightMousePress = std::function<void(int)>([](int keyCode)
		{
			if (instance != nullptr)
			{
				instance->rightMousePressed = true;
				WindowManager::GetWindow("Engine")->DisableCursor();
				WindowManager::GetWindow("Engine")->SetCursorPosition(lookPreservationCursorPosition);
			}
		});

	static std::function<void(int)> rightMouseRelease = std::function<void(int)>([](int keyCode)
		{
			if (instance != nullptr)
			{
				instance->rightMousePressed = false;
				lookPreservationCursorPosition = WindowManager::GetWindow("Engine")->GetCursorPosition();
				WindowManager::GetWindow("Engine")->EnableCursor();
			}
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

						int y = (int)gridPos.y;

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

						int y = (int)gridPos.y;

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
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_ESCAPE, &escPress, "hiddenCursor");
	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_2, &rightMousePress, "MouseLook");
	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_2, &rightMouseRelease, "MouseLook");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_W, &wPress, "FreeCamera");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_A, &aPress, "FreeCamera");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_S, &sPress, "FreeCamera");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_D, &dPress, "FreeCamera");

}

void Editor::InitializeGrid()
{
	Model* gridModel = ModelManager::CreateModelTerrain("EditorGrid", "Assets/Texture/planeHeightMap.png", 500.0f, 500.0f, 500U, 500U, 0, 0.0f);

	grid = GraphicsObjectManager::CreateGO3DColored(gridModel, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	grid->SetDrawMode(GO3D::Mode::LINE);
	grid->SetLineWidth(1.0f);
}

void Editor::TerminateGrid()
{
	GraphicsObjectManager::Delete(grid);
}

void Editor::FreeCameraMovement()
{
	if (rightMousePressed)
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
}
