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
			

			LineSegment3D lineFromScreenToWorld = Camera::CastLineFromCursorWithActiveCamera();

			const glm::vec3 normal = glm::normalize(lineFromScreenToWorld.GetEnd() - lineFromScreenToWorld.GetStart());

			Ray ray(lineFromScreenToWorld.GetStart(), normal);

			float gridY = Editor::GetGridY();
			float selectionY = instance->selection->GetPosition().y;

			Editor::SetGridY(selectionY);

			Plane plane(glm::vec3(0.0f, 1.0f, 0.0f), selectionY);

			glm::vec3 planePoint = lineFromScreenToWorld.GetStart() + normal * plane.RayIntersect(ray);

			glm::vec3 selectionCurrentPosition = instance->selection->GetPosition();

			if (Editor::ShiftPressed())
			{
				int newPosX = (int)(planePoint.x + instance->selectionPlanePointDelta.x), newPosY = (int)(planePoint.y + instance->selectionPlanePointDelta.y), newPosZ = (int)(planePoint.z + instance->selectionPlanePointDelta.z);
				instance->selection->SetPosition({newPosX, newPosY, newPosZ});
			}
			else
			{
				instance->selection->SetPosition(planePoint + instance->selectionPlanePointDelta);
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

		static std::function<void(double, double)> mouseScroll = std::function<void(double, double)>([](double xScroll, double yScroll)
			{
				if (instance != nullptr)
				{
					float speed = 100.0f;
					speed *= yScroll;
					if (instance->selection != nullptr)
					{
						instance->selection->SetPosition(instance->selection->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * TimeManager::DeltaTime() * speed);
					}
				}
			});

		InputManager::RegisterCallbackForMouseScroll(&mouseScroll, "SelectionManager");
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

							LineSegment3D lineFromCamera = Camera::CastLineFromCursorWithActiveCamera();

							const glm::vec3 normal = glm::normalize(glm::vec3(lineFromCamera.GetEnd()) - lineFromCamera.GetStart());

							Ray ray(lineFromCamera.GetStart(), normal);

							float selectionY = selection->GetPosition().y;

							Plane plane(glm::vec3(0.0f, 1.0f, 0.0f), selectionY);

							glm::vec3 planePoint = lineFromCamera.GetStart() + normal * plane.RayIntersect(ray);

							selectionPlanePointDelta = selection->GetPosition() - planePoint;
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

	static std::function<void()> onEditorEnable = std::function<void()>([this]()
		{
		});

	Editor::RegisterOnEditorDisable(&onEditorDisable);
	Editor::RegisterOnEditorEnable(&onEditorEnable);

	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, &click, "Selection");
	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_1, &clickRelease, "Selection");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_Q, &qPress, "Selection");
	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESSED, KEY_E, &ePress, "Selection");
}
