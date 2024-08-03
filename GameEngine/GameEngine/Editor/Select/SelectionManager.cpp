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

			Plane plane(glm::vec3(0.0f, 1.0f, 0.0f), Editor::GetGridY());

			glm::vec3 planePoint = cam.GetPosition() + normal * plane.RayIntersect(ray);



			if (Editor::ShiftPressed())
			{
				int newPosX = planePoint.x, newPosY = planePoint.y, newPosZ = planePoint.z;
				instance->selection->SetPosition({newPosX, newPosY, newPosZ});
			}
			else
			{
				instance->selection->SetPosition(planePoint);
			}
		}
	}
}

SelectionManager::SelectionManager() :
	selection(nullptr)
{
	SetupInput();
}

SelectionManager::~SelectionManager()
{
}

void SelectionManager::SetupInput()
{
	static std::function<void(int)> click = std::function<void(int)>([this](int mouseButton)
		{
			
			std::vector<Scene*> scenes = SceneManager::GetLoadedScenes();

			float closestSelection = FLT_MAX;

			for (Scene* const scene : scenes)
			{
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

	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, &click, "Selection");
	InputManager::EditorRegisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_1, &clickRelease, "Selection");
}
