#include "SelectionManager.h"

#include "../../Utils/SingletonHelpers.h"
#include "../../Input/InputManager.h"
#include "../../Scene/SceneManager.h"
#include "../../Scene/Scene.h"
#include "../../Renderer/Window/WindowManager.h"
#include "../../Renderer/Camera/CameraManager.h"
#include "../../Renderer/Camera/Camera.h"
#include "../../Math/Shapes/LineSegment3D.h"
#include "../../Math/Shapes/Ray.h"
#include "../../Math/Shapes/Plane.h"
#include "../../GameObject/GameObject.h"
#include "../../Editor/Editor.h"
#include "../../Renderer/Text/Text.h"
#include "../../Time/TimeManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SelectionManager* SelectionManager::instance = nullptr;

void SelectionManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "SelectionManager");
}

void SelectionManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "SelectionManager");
}

void SelectionManager::Update()
{
	if (instance != nullptr)
	{
		if (instance->selection != nullptr)
		{

			std::string newText = "Selection: " + instance->selection->GetName();
			if (instance->selectionName != nullptr)
			{
				if (instance->selectionName->GetString() != newText)
				{
					delete instance->selectionName;
					instance->selectionName = new Text(newText, "arial", { 1.0f, 1.0f, 1.0f, 1.0f }, glm::vec2(0.0f, WindowManager::GetWindow("Engine")->GetHeight() - 100.f), glm::vec2(0.05f));
				}
			}
			else
			{
				instance->selectionName = new Text(newText, "arial", { 1.0f, 1.0f, 1.0f, 1.0f }, glm::vec2(0.0f, WindowManager::GetWindow("Engine")->GetHeight() - 100.f), glm::vec2(0.05f));
			}
			

			Camera& cam = CameraManager::GetActiveCamera();

			// Screen space to world space for object picking.
			Window* window = WindowManager::GetWindow("Engine");
			glm::vec2 cursorPos = window->GetCursorPosition();
			glm::mat4 invPersp = glm::inverse(cam.GetProjection());
			glm::mat4 invView = glm::inverse(cam.GetView());
			glm::mat4 screenToNDC(
				glm::vec4((float)window->GetWidth() / 2.0f, 0.0f, 0.0f, 0.0f),
				glm::vec4(0, -(float)window->GetHeight() / 2.0f, 0.0f, 0.0f),
				glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
				glm::vec4((float)window->GetWidth() / 2.0f, (float)window->GetHeight() / 2.0f, 0.5f, 1.0f)
			);
			screenToNDC = glm::inverse(screenToNDC);

			glm::vec4 x = glm::vec4(cursorPos.x, cursorPos.y, 1.0f, 1.0f);
			x = screenToNDC * x;
			x = invPersp * x;
			x = invView * x;
			x /= x.w;

			LineSegment3D lineFromScreenToWorld(cam.GetPosition(), x);

			const glm::vec3 normal = glm::normalize(glm::vec3(x) - cam.GetPosition());

			Ray ray(cam.GetPosition(), normal);

			float gridY = Editor::GetGridY();
			float selectionY = instance->selection->GetPosition().y;

			Editor::SetGridY(selectionY);

			Plane plane(glm::vec3(0.0f, 1.0f, 0.0f), selectionY);

			glm::vec3 planePoint = cam.GetPosition() + normal * plane.RayIntersect(ray);

			if (Editor::ShiftPressed())
			{
				int newPosX = (int)planePoint.x, newPosY = (int)planePoint.y, newPosZ = (int)planePoint.z;
				instance->selection->SetPosition({newPosX, newPosY, newPosZ});
			}
			else
			{
				instance->selection->SetPosition(planePoint);
			}
		}
		else
		{
			std::string newText = "Selection: ";

			if (instance->selectionName != nullptr)
			{
				if (instance->selectionName->GetString() != newText)
				{
					delete instance->selectionName;
					instance->selectionName = new Text(newText, "arial", { 1.0f, 1.0f, 1.0f, 1.0f }, glm::vec2(0.0f, WindowManager::GetWindow("Engine")->GetHeight() - 100.f), glm::vec2(0.05f));
				}
			}
			else
			{
				instance->selectionName = new Text(newText, "arial", { 1.0f, 1.0f, 1.0f, 1.0f }, glm::vec2(0.0f, WindowManager::GetWindow("Engine")->GetHeight() - 100.f), glm::vec2(0.05f));
			}
			
		}
	}
}

GameObject* const SelectionManager::GetSelection()
{
	if (instance != nullptr)
	{
		return instance->selection;
	}

	return nullptr;
}

void SelectionManager::ClearSelection()
{
	if (instance != nullptr)
	{
		instance->selection = nullptr;
	}
}

SelectionManager::SelectionManager() :
	selection(nullptr),
	selectionName(nullptr)
{
	selectionName = new Text("Selection: ", "arial", {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2(0.0f, WindowManager::GetWindow("Engine")->GetHeight() - 100.f), glm::vec2(0.05f));
	SetupInput();
}

SelectionManager::~SelectionManager()
{
	if (selectionName != nullptr)
	{
		delete selectionName;
	}
}

void SelectionManager::SetupInput()
{
	static std::function<void(int)> click = std::function<void(int)>([this](int mouseButton)
		{
			
			std::vector<Scene*> scenes = SceneManager::GetRegisteredScenes();

			float closestSelection = FLT_MAX;

			for (Scene* const scene : scenes)
			{
				if (!scene->Initialized())
				{
					continue;
				}

				const std::unordered_map<std::string, GameObject*>& sceneObjects = scene->GetGameObjects();

				for (auto& object : sceneObjects)
				{
					if (object.second->Hovered())
					{
						float distanceFromCamera = glm::length(CameraManager::GetActiveCamera().GetPosition() - object.second->GetPosition());

						if (closestSelection > distanceFromCamera)
						{
							selection = object.second;
						}
					}
				}
			}

			
		});

	static std::function<void(int)> clickRelease = std::function<void(int)>([this](int mouseButton)
		{
			selection = nullptr;
		});

	static std::function<void(int)> qPress = std::function<void(int)>([this](int keyCode)
		{

			if (selection != nullptr)
			{
				glm::mat4 rotation = selection->GetRotation();

				rotation = glm::rotate(rotation, 1.0f * TimeManager::DeltaTime(), glm::vec3(0.0f, 1.0f, 0.0f));

				selection->SetRotation(rotation);
			}

		});

	static std::function<void(int)> ePress = std::function<void(int)>([this](int keyCode)
		{

			if (selection != nullptr)
			{
				glm::mat4 rotation = selection->GetRotation();

				rotation = glm::rotate(rotation, -1.0f * TimeManager::DeltaTime(), glm::vec3(0.0f, 1.0f, 0.0f));

				selection->SetRotation(rotation);
			}

		});

	static std::function<void()> onEditorDisable = std::function<void()>([this]()
		{
			delete selectionName;
			selectionName = nullptr;
		});

	Editor::RegisterOnEditorDisable(&onEditorDisable);

	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, &click, "Selection");
	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_1, &clickRelease, "Selection");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_Q, &qPress, "Selection");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_E, &ePress, "Selection");
}
