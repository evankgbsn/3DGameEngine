#include "EditorUI.h"

#include "../../UI/Button.h"
#include "../../Renderer/Texture/TextureManager.h"
#include "../../Editor/Editor.h"
#include "../../Renderer/Window/WindowManager.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Model/Vertex.h"
#include "../../UI/InputField.h"
#include "../../Scene/SceneManager.h"
#include "../../Scene/Scene.h"
#include "../../Math/Math.h"
#include "../../UI/TextField.h"
#include "../../Input/InputManager.h"
#include "../../UI/Sprite.h"
#include "../GameObject/GameObject.h"
#include "../../Renderer/Camera/CameraManager.h"
#include "../../Renderer/Camera/Camera.h"
#include "../../Math/Shapes/Ray.h"
#include "../../Math/Shapes/Plane.h"
#include "../Editor/Select/SelectionManager.h"

EditorUI::EditorUI() :
	disabled(false),
	sceneManagementInterfaceEnabled(true),
	objectManagementInterfaceEnabled(true),
	altPress(nullptr),
	sPress(nullptr)
{
	CreatePlayButton();
	CreateSceneManagementInterface();
	CreateInterfaceToggleCallbacks();
	CreateObjectManagementInterface();
}

EditorUI::~EditorUI()
{
	CleanupObjectManagementInterface();
	CleanupInterfaceToggleCallbacks();
	CleanupSceneManagementInterface();

	if (playButton != nullptr)
	{
		delete playButton;
	}
}

void EditorUI::Update()
{
	if (playButton != nullptr)
	{
		playButton->Update();
	}

	UpdateSceneManagementInterface();
	UpdateObjectManagementInterface();
}

bool EditorUI::IsDisabled() const
{
	return disabled;
}

void EditorUI::CreatePlayButton()
{
	TextureManager::LoadTexture("Assets/Texture/Green.png", "Play");
	TextureManager::LoadTexture("Assets/Texture/grey.png", "PlayPress");
	TextureManager::LoadTexture("Assets/Texture/grey.png", "PlayIdle");
	ModelManager::LoadModel("Play", "Assets/Model/PlayButton.gltf", false);

	static std::function<void()> buttonFunc = std::function<void()>([this]()
		{
			Editor::Disable();
		});

	playButton = new Button("PlayIdle", "Play", "PlayPress", "Play", { 0.5f, 0.9f }, &buttonFunc);
	playButton->SetScale(playButton->GetScale() / 2.0f);
}

void EditorUI::CreateLoadSceneInputFields()
{
	TextureManager::LoadTexture("Assets/Texture/Black.png", "LoadSceneInputFieldBackground");
	TextureManager::LoadTexture("Assets/Texture/Black.png", "LoadSceneInputFieldBackgroundHover");
	TextureManager::LoadTexture("Assets/Texture/Green.png", "LoadSceneInputFieldBackgroundPress");

	Window* window = WindowManager::GetWindow("Engine");
	float windowHeight = (float)window->GetHeight();
	float windowWidth = (float)window->GetWidth();

	loadSceneInputField0 = new InputField("LoadSceneInputFieldBackground", "LoadSceneInputFieldBackgroundHover", "LoadSceneInputFieldBackgroundPress", { 0.15625, 0.037037037037037 }, { 0.175f, 0.37f + 0.02f }, nullptr, nullptr, true);
	loadSceneInputField1 = new InputField("LoadSceneInputFieldBackground", "LoadSceneInputFieldBackgroundHover", "LoadSceneInputFieldBackgroundPress", { 0.15625, 0.037037037037037 }, { 0.175f, 0.45f + 0.02f }, nullptr, nullptr, true);

	loadSceneInputField0->SetZ(0.2f);
	loadSceneInputField1->SetZ(0.2f);

	loadSceneInputFieldOnEnter = new std::function<void()>([this]()
		{
			std::string sceneName = loadSceneInputField1->GetText();
			std::string sceneFile = loadSceneInputField0->GetText();

			Scene* scene = SceneManager::GetRegisteredScene(sceneName);
			if (scene != nullptr)
			{
				if (!SceneManager::SceneLoaded(sceneName))
				{
					SceneManager::LoadScene(sceneName);
					SceneManager::InitializeScene(sceneName);
				}

				scene->Deserialize("Assets/Scenes/" + sceneFile);
			}
		});

	loadSceneInputField0->SetOnEnter(loadSceneInputFieldOnEnter);

	unloadSceneInputField = new InputField("LoadSceneInputFieldBackground", "LoadSceneInputFieldBackgroundHover", "LoadSceneInputFieldBackgroundPress", { 0.15625, 0.037037037037037 }, { 0.175f, 0.57f + 0.02f }, nullptr, nullptr, true);
	unloadSceneInputField->SetZ(0.2f);

	unloadSceneInputFieldOnEnter = new std::function<void()>([this]()
		{
			std::string sceneName = unloadSceneInputField->GetText();

			Scene* scene = SceneManager::GetRegisteredScene(sceneName);

			if (scene != nullptr)
			{
				if (scene->Loaded())
				{
					SceneManager::TerminateScene(sceneName);
					SceneManager::UnloadScene(sceneName);
				}
			}
		});

	unloadSceneInputField->SetOnEnter(unloadSceneInputFieldOnEnter);

	saveSceneInputFieldFile = new InputField("LoadSceneInputFieldBackground", "LoadSceneInputFieldBackgroundHover", "LoadSceneInputFieldBackgroundPress", { 0.15625, 0.037037037037037 }, { 0.175f, 0.16f + 0.02f }, nullptr, nullptr, true);
	saveSceneInputFieldName = new InputField("LoadSceneInputFieldBackground", "LoadSceneInputFieldBackgroundHover", "LoadSceneInputFieldBackgroundPress", { 0.15625, 0.037037037037037 }, { 0.175f, 0.24f + 0.02f }, nullptr, nullptr, true);

	saveSceneInputFieldFile->SetZ(0.2f);
	saveSceneInputFieldName->SetZ(0.2f);

	saveSceneInputFieldOnEnter = new std::function<void()>([this]()
		{
			std::string fileName = saveSceneInputFieldFile->GetText();
			std::string sceneName = saveSceneInputFieldName->GetText();

			Scene* scene = SceneManager::GetRegisteredScene(sceneName);

			if (scene != nullptr)
			{
				if (scene->Initialized())
				{
					scene->Save(fileName);
				}
			}

		});

	saveSceneInputFieldFile->SetOnEnter(saveSceneInputFieldOnEnter);

}

void EditorUI::CreateSceneManagementInterface()
{
	CreateLoadSceneInputFields();

	Window* window = WindowManager::GetWindow("Engine");

	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());

	// Create the background.
	float xDimension = 500.0f / width;
	float yDimension = 750.0f / height;
	float z = 0.02f;

	Texture* sceneManagementBackgroundTexture = TextureManager::LoadTexture("Assets/Texture/grey.png", "SceneManagementBackground");

	float backgroundXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.185f * width);
	float backgroundYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.4f * height);

	sceneManagementBackground = new Sprite("SceneManagementBackground", { backgroundXPos, backgroundYPos }, {xDimension, yDimension});
	sceneManagementBackground->SetZ(z);

	// Create title.
	float titleXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.065f * width);
	float titleYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.7f * height);

	sceneManagementWindowTitle = new TextField("Scene Management", "exo2", { titleXPos, titleYPos }, { 25.0f, 25.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
	sceneManagementWindowTitle->SetZ(0.5f);

	// Create input field titles.
	float sceneNameXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.1f * width);
	float sceneNameYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.5f * height);

	sceneName = new TextField("Scene Name:", "exo2", { sceneNameXPos, sceneNameYPos }, {12.5f, 12.5f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	sceneName->SetZ(0.5f);

	float sceneFileXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.1f * width);
	float sceneFileYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.42f * height);

	sceneFileName = new TextField("Scene File:", "exo2", { sceneFileXPos, sceneFileYPos }, {12.5f, 12.5f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	sceneFileName->SetZ(0.5f);

	// Create load scene title.
	float loadSceneXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.07f * width);
	float loadSceneYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.53f * height);

	sceneLoadingTitle = new TextField("Load Scene:", "exo2", { loadSceneXPos, loadSceneYPos }, {15.0f, 15.0f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	sceneLoadingTitle->SetZ(0.5f);

	// Create unload scene title.
	float unloadingSceneXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.07f * width);
	float unloadingSceneYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.65f * height);

	sceneUnloadingTitle = new TextField("Unload Scene:", "exo2", { unloadingSceneXPos, unloadingSceneYPos }, {15.0f, 15.0f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	sceneUnloadingTitle->SetZ(0.5f);


	// Create unload scene name.
	float unloadingSceneNameXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.1f * width);
	float unloadingSceneNameYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.62f * height);

	unloadSceneName = new TextField("SceneName:", "exo2", { unloadingSceneNameXPos, unloadingSceneNameYPos }, {12.5f, 12.5f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	unloadSceneName->SetZ(0.5f);

	// Create save scene title.
	float saveSceneXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.07f * width);
	float saveSceneYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.32f * height);

	saveSceneTitle = new TextField("Save Scene:", "exo2", { saveSceneXPos, saveSceneYPos }, {15.0f, 15.0f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	saveSceneTitle->SetZ(0.5f);

	// Create save scene name.
	float saveSceneNameXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.1f * width);
	float saveSceneNameYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.29f * height);

	saveSceneName = new TextField("SceneName:", "exo2", { saveSceneNameXPos, saveSceneNameYPos }, {12.5f, 12.5f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	saveSceneName->SetZ(0.5f);

	// Create save scene file.
	float saveSceneFileXPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, 0.1f * width);
	float saveSceneFileYPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, 0.21f * height);

	saveSceneFile = new TextField("SceneFile:", "exo2", { saveSceneFileXPos, saveSceneFileYPos }, {12.5f, 12.5f}, { 1.0f, 1.0f, 1.0f, 1.0f });
	saveSceneFile->SetZ(0.5f);


}

void EditorUI::UpdateSceneManagementInterface()
{
	if (sceneManagementInterfaceEnabled)
	{
		if (loadSceneInputField0 != nullptr)
		{
			loadSceneInputField0->Update();
		}

		if (loadSceneInputField1 != nullptr)
		{
			loadSceneInputField1->Update();
		}

		if (unloadSceneInputField != nullptr)
		{
			unloadSceneInputField->Update();
		}

		if (saveSceneInputFieldFile != nullptr)
		{
			saveSceneInputFieldFile->Update();
		}

		if (saveSceneInputFieldName != nullptr)
		{
			saveSceneInputFieldName->Update();
		}
	}
}

void EditorUI::CleanupSceneManagementInterface()
{
	if (loadSceneInputField0 != nullptr)
	{
		delete loadSceneInputField0;
	}

	if (loadSceneInputField1 != nullptr)
	{
		delete loadSceneInputField1;
	}

	if (loadSceneInputFieldOnEnter != nullptr)
	{
		delete loadSceneInputFieldOnEnter;
	}

	if (sceneManagementBackground != nullptr)
	{
		delete sceneManagementBackground;
	}

	if (sceneManagementWindowTitle != nullptr)
	{
		delete sceneManagementWindowTitle;
	}

	if (sceneName != nullptr)
	{
		delete sceneName;
	}

	if (sceneFileName != nullptr)
	{
		delete sceneFileName;
	}

	if (sceneLoadingTitle != nullptr)
	{
		delete sceneLoadingTitle;
	}

	if (unloadSceneInputField != nullptr)
	{
		delete unloadSceneInputField;
	}

	if (unloadSceneInputFieldOnEnter != nullptr)
	{
		delete unloadSceneInputFieldOnEnter;
	}

	if (sceneUnloadingTitle != nullptr)
	{
		delete sceneUnloadingTitle;
	}

	if (unloadSceneName != nullptr)
	{
		delete unloadSceneName;
	}

	if (saveSceneInputFieldFile != nullptr)
	{
		delete saveSceneInputFieldFile;
	}

	if (saveSceneInputFieldName != nullptr)
	{
		delete saveSceneInputFieldName;
	}

	if (saveSceneInputFieldOnEnter != nullptr)
	{
		delete saveSceneInputFieldOnEnter;
	}

	if (saveSceneTitle != nullptr)
	{
		delete saveSceneTitle;
	}

	if (saveSceneName != nullptr)
	{
		delete saveSceneName;
	}

	if (saveSceneFile != nullptr)
	{
		delete saveSceneFile;
	}
}

void EditorUI::EnableSceneManagementInterface()
{
	if (!sceneManagementInterfaceEnabled)
	{
		if (loadSceneInputField0 != nullptr)
		{
			loadSceneInputField0->Enable();
		}

		if (loadSceneInputField1 != nullptr)
		{
			loadSceneInputField1->Enable();
		}

		if (sceneManagementBackground != nullptr)
		{
			sceneManagementBackground->Enable();
		}

		if (sceneManagementWindowTitle != nullptr)
		{
			sceneManagementWindowTitle->Enable();
		}

		if (sceneName != nullptr)
		{
			sceneName->Enable();
		}

		if (sceneFileName != nullptr)
		{
			sceneFileName->Enable();
		}

		if (sceneLoadingTitle != nullptr)
		{
			sceneLoadingTitle->Enable();
		}

		if (unloadSceneInputField != nullptr)
		{
			unloadSceneInputField->Enable();
		}

		if (sceneUnloadingTitle != nullptr)
		{
			sceneUnloadingTitle->Enable();
		}

		if (unloadSceneName != nullptr)
		{
			unloadSceneName->Enable();
		}

		if (saveSceneInputFieldFile != nullptr)
		{
			saveSceneInputFieldFile->Enable();
		}

		if (saveSceneInputFieldName != nullptr)
		{
			saveSceneInputFieldName->Enable();
		}

		if (saveSceneTitle != nullptr)
		{
			saveSceneTitle->Enable();
		}

		if (saveSceneName != nullptr)
		{
			saveSceneName->Enable();
		}

		if (saveSceneFile != nullptr)
		{
			saveSceneFile->Enable();
		}

		sceneManagementInterfaceEnabled = true;
	}
}

void EditorUI::DisableSceneManagementInterface()
{
	if (sceneManagementInterfaceEnabled)
	{
		if (loadSceneInputField0 != nullptr)
		{
			loadSceneInputField0->Disable();
		}

		if (loadSceneInputField1 != nullptr)
		{
			loadSceneInputField1->Disable();
		}

		if (sceneManagementBackground != nullptr)
		{
			sceneManagementBackground->Disable();
		}

		if (sceneManagementWindowTitle != nullptr)
		{
			sceneManagementWindowTitle->Disable();
		}

		if (sceneName != nullptr)
		{
			sceneName->Disable();
		}

		if (sceneFileName != nullptr)
		{
			sceneFileName->Disable();
		}

		if (sceneLoadingTitle != nullptr)
		{
			sceneLoadingTitle->Disable();
		}

		if (unloadSceneInputField != nullptr)
		{
			unloadSceneInputField->Disable();
		}

		if (sceneUnloadingTitle != nullptr)
		{
			sceneUnloadingTitle->Disable();
		}

		if (unloadSceneName != nullptr)
		{
			unloadSceneName->Disable();
		}

		if (saveSceneInputFieldFile != nullptr)
		{
			saveSceneInputFieldFile->Disable();
		}

		if (saveSceneInputFieldName != nullptr)
		{
			saveSceneInputFieldName->Disable();
		}

		if (saveSceneTitle != nullptr)
		{
			saveSceneTitle->Disable();
		}

		if (saveSceneName != nullptr)
		{
			saveSceneName->Disable();
		}

		if (saveSceneFile != nullptr)
		{
			saveSceneFile->Disable();
		}

		sceneManagementInterfaceEnabled = false;
	}
}

void EditorUI::CreateInterfaceToggleCallbacks()
{
	altPress = new std::function<void(int)>([this](int keyCode)
		{
			altPressed = true;

			InputManager::EditorRegisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_ALT, altRelease, "EditorUI");
			InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_ALT, "EditorUI");

		});

	altRelease = new std::function<void(int)>([this](int keyCode)
		{
			altPressed = false;

			InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_ALT, altPress, "EditorUI");
			InputManager::EditorDeregisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_ALT, "EditorUI");
		});

	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_ALT, altPress, "EditorUI");

	sPress = new std::function<void(int)>([this](int keyCode)
		{
			if (altPressed)
			{
				if (sceneManagementInterfaceEnabled)
				{
					DisableSceneManagementInterface();
				}
				else
				{
					EnableSceneManagementInterface();
				}
			}
		});

	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_S, sPress, "EditorUI");
	
	oPress = new std::function<void(int)>([this](int keyCode)
		{
			if (altPressed)
			{
				if (objectManagementInterfaceEnabled)
				{
					DisableObjectManagementInterface();
				}
				else
				{
					EnableObjectManagementInterface();
				}
			}
		});

	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_O, oPress, "EditorUI");
}

void EditorUI::CleanupInterfaceToggleCallbacks()
{
	InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_S, "EditorUI");
	InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_O, "EditorUI");

	if (altPressed)
	{
		InputManager::EditorDeregisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_ALT, "EditorUI");
	}
	else
	{
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_ALT, "EditorUI");
	}

	if (altPress != nullptr)
	{
		delete altPress;
		altPress = nullptr;
	}

	if (altRelease != nullptr)
	{
		delete altRelease;
		altRelease = nullptr;
	}

	if (sPress != nullptr)
	{
		delete sPress;
		sPress = nullptr;
	}

	if (oPress != nullptr)
	{
		delete oPress;
		oPress = nullptr;
	}

}

void EditorUI::CreateObjectManagementInterface()
{
	// Create the background.
	Texture* sceneManagementBackgroundTexture = TextureManager::LoadTexture("Assets/Texture/grey.png", "ObjectManagementBackground");
	objectManagementBackground = new Sprite("ObjectManagementBackground", { 0.8f, 0.5f }, { .3f, .5f });
	objectManagementBackground->SetZ(0.02f);

	// Create the title
	objectManagementTitle = new TextField("Object Management", "exo2", { 0.675f, 0.7f }, { 25.0f, 25.0f }, {0.0f, 0.0f, 0.0f, 1.0f});
	objectManagementTitle->SetZ(0.5f);

	// Create the create object section
	objectManagementCreateTitle = new TextField("Create:", "exo2", { 0.69f, 0.65f }, { 15.0f, 15.0f }, glm::vec4(1.0f));
	objectManagementCreateTitle->SetZ(0.5f);

	// Create the create object name title for input field.
	objectManagementCreateObjectName = new TextField("Object Class Name:", "exo2", { 0.72f, 0.608f }, { 12.5f, 12.5f }, glm::vec4(1.0f));
	objectManagementCreateObjectName->SetZ(0.5f);

	// Create the create object name input field.
	TextureManager::LoadTexture("Assets/Texture/Black.png", "CreateObjectInputFieldBackground");
	TextureManager::LoadTexture("Assets/Texture/Black.png", "CreateObjectInputFieldBackgroundHover");
	TextureManager::LoadTexture("Assets/Texture/Green.png", "CreateObjectInputFieldBackgroundPress");

	objectManagementCreateObjectOnEnter = new std::function<void()>([this]()
		{
			std::string objectClass;
			std::string parentSceneName;
			
			if (objectManagementCreateObjectNameInput != nullptr)
			{
				objectClass = objectManagementCreateObjectNameInput->GetText();
			}

			if (objectManagementCreateObjectParentSceneNameInput != nullptr)
			{
				parentSceneName = objectManagementCreateObjectParentSceneNameInput->GetText();
			}

			GameObject* newObject = nullptr;
			GameObject::GetConstructor(objectClass)(&newObject);

			if (newObject != nullptr)
			{
				Scene* parentScene = SceneManager::GetRegisteredScene(parentSceneName);
				
				if (parentScene != nullptr)
				{
					const std::unordered_map <std::string, GameObject*>& gameObjects = parentScene->GetGameObjects();
					std::string newObjectName = newObject->GetNameOfType() + std::to_string(newObject->GetID());
					std::string takenName;
					
					for (const auto& object : gameObjects)
					{
						if (object.first == newObjectName)
						{
							takenName = newObjectName;
							break;
						}
					}

					unsigned long long id = 0;
					while (newObjectName == takenName)
					{
						for (const auto& object : gameObjects)
						{
							if (object.first == newObjectName)
							{
								newObjectName = newObject->GetNameOfType() + std::to_string(newObject->GetID() + id++);
								break;
							}
						}
					}

					parentScene->RegisterGameObject(newObject, newObjectName);
					parentScene->AddToCreatedObjects(newObjectName, newObject);

					Camera& cam = CameraManager::GetActiveCamera();
					
					Ray ray(cam.GetPosition(), cam.GetForwardVector());

					Plane plane(glm::vec3(0.0f, 1.0f, 0.0f), Editor::GetGridY());

					glm::vec3 planePoint = cam.GetPosition() + cam.GetForwardVector() * plane.RayIntersect(ray);

					newObject->SetPosition(planePoint);
				}
			}
		});

	objectManagementCreateObjectNameInput = new InputField(
		"CreateObjectInputFieldBackground",
		"CreateObjectInputFieldBackgroundHover",
		"CreateObjectInputFieldBackgroundPress",
		{ 0.15625, 0.037037037037037 },
		{ 0.8f, 0.575f},
		objectManagementCreateObjectOnEnter,
		nullptr,
		true
	);

	objectManagementCreateObjectNameInput->SetZ(0.5f);

	// Create the create object parent scene name title.
	objectManagementCreateObjectParentSceneName = new TextField("Object Parent Scene:", "exo2", { 0.72f, 0.53f }, { 12.5f, 12.5f }, glm::vec4(1.0f));
	objectManagementCreateObjectParentSceneName->SetZ(0.5f);

	// Create the create object parent scene name input field
	objectManagementCreateObjectParentSceneNameInput = new InputField(
		"CreateObjectInputFieldBackground",
		"CreateObjectInputFieldBackgroundHover",
		"CreateObjectInputFieldBackgroundPress",
		{ 0.15625, 0.037037037037037 },
		{ 0.8f, 0.5f },
		objectManagementCreateObjectOnEnter,
		nullptr,
		true
	);
	
	objectManagementCreateObjectParentSceneNameInput->SetZ(0.5f);

	// Create selected object text
	objectManagementSelectedObject = new TextField("Selection: ", "exo2", { 0.72f, 0.45f }, { 12.5f, 12.5f }, glm::vec4(1.0f));
	objectManagementSelectedObject->SetZ(0.5f);

	// Crete the delete object title text
	objectManagementDeleteTitle = new TextField("Delete:", "exo2", { 0.69f, 0.4f }, { 15.0f, 15.0f }, glm::vec4(1.0f));
	objectManagementDeleteTitle->SetZ(0.5f);

	objectManagementDeleteNameTitle = new TextField("Object Name:", "exo2", { 0.72f, 0.37f }, { 12.5f, 12.5f }, glm::vec4(1.0f));
	objectManagementDeleteNameTitle->SetZ(0.5f);

	objectManagementDeleteObjectOnEnter = new std::function<void()>([this]()
		{
			std::string objectName = objectManagementDeleteName->GetText();

			const std::vector<Scene*>& registeredScenes = SceneManager::GetRegisteredScenes();

			for (const auto& scene : registeredScenes)
			{
				if (scene->Loaded())
				{
					if (scene->GetGameObject(objectName) != nullptr)
					{
						scene->RemoveFromCreatedObjects(objectName);
					}
				}
			}
		});

	objectManagementDeleteName = new InputField(
		"CreateObjectInputFieldBackground",
		"CreateObjectInputFieldBackgroundHover",
		"CreateObjectInputFieldBackgroundPress",
		{ 0.15625, 0.037037037037037 },
		{ 0.8f, 0.34f },
		objectManagementDeleteObjectOnEnter,
		nullptr,
		true
	);

	objectManagementDeleteName->SetZ(0.5f);
}

void EditorUI::CleanupObjectManagementInterface()
{
	if (objectManagementDeleteName != nullptr)
	{
		delete objectManagementDeleteName;
	}

	if (objectManagementDeleteObjectOnEnter != nullptr)
	{
		delete objectManagementDeleteObjectOnEnter;
	}

	if (objectManagementDeleteNameTitle != nullptr)
	{
		delete objectManagementDeleteNameTitle;
	}

	if (objectManagementDeleteTitle != nullptr)
	{
		delete objectManagementDeleteTitle;
	}

	if (objectManagementCreateObjectParentSceneNameInput != nullptr)
	{
		delete objectManagementCreateObjectParentSceneNameInput;
	}

	if (objectManagementCreateObjectParentSceneName != nullptr)
	{
		delete objectManagementCreateObjectParentSceneName;
	}

	if (objectManagementCreateObjectName != nullptr)
	{
		delete objectManagementCreateObjectName;
	}

	if (objectManagementCreateTitle)
	{
		delete objectManagementCreateTitle;
	}
	
	if (objectManagementTitle != nullptr)
	{
		delete objectManagementTitle;
	}

	if (objectManagementCreateObjectName != nullptr)
	{
		delete objectManagementBackground;
	}
}

void EditorUI::UpdateObjectManagementInterface()
{
	if (objectManagementInterfaceEnabled)
	{
		if (objectManagementCreateObjectNameInput != nullptr)
		{
			objectManagementCreateObjectNameInput->Update();
		}

		if (objectManagementDeleteName != nullptr)
		{
			objectManagementDeleteName->Update();
		}

		if (objectManagementCreateObjectParentSceneNameInput != nullptr)
		{
			objectManagementCreateObjectParentSceneNameInput->Update();
		}

		if (SelectionManager::GetSelection() != nullptr)
		{
			if (objectManagementSelectedObject->GetString() != "Selection: " + SelectionManager::GetSelection()->GetName())
			{
				objectManagementSelectedObject->SetText("Selection: " + SelectionManager::GetSelection()->GetName());
			}
		}
		else
		{
			objectManagementSelectedObject->SetText("Selection: ");
		}
	}
}

void EditorUI::EnableObjectManagementInterface()
{
	if (!objectManagementInterfaceEnabled)
	{
		if (objectManagementDeleteTitle != nullptr)
		{
			objectManagementDeleteTitle->Enable();
		}

		if (objectManagementDeleteNameTitle != nullptr)
		{
			objectManagementDeleteNameTitle->Enable();
		}

		if (objectManagementDeleteName != nullptr)
		{
			objectManagementDeleteName->Enable();
		}

		if (objectManagementBackground != nullptr)
		{
			objectManagementBackground->Enable();
		}

		if (objectManagementTitle != nullptr)
		{
			objectManagementTitle->Enable();
		}

		if (objectManagementCreateTitle != nullptr)
		{
			objectManagementCreateTitle->Enable();
		}

		if (objectManagementCreateObjectName != nullptr)
		{
			objectManagementCreateObjectName->Enable();
		}

		if (objectManagementCreateObjectNameInput != nullptr)
		{
			objectManagementCreateObjectNameInput->Enable();
		}

		if (objectManagementCreateObjectParentSceneName != nullptr)
		{
			objectManagementCreateObjectParentSceneName->Enable();
		}

		if (objectManagementCreateObjectParentSceneNameInput != nullptr)
		{
			objectManagementCreateObjectParentSceneNameInput->Enable();
		}

		if (objectManagementSelectedObject != nullptr)
		{
			objectManagementSelectedObject->Enable();
		}
	}

	objectManagementInterfaceEnabled = true;
}

void EditorUI::DisableObjectManagementInterface()
{
	if (objectManagementInterfaceEnabled)
	{
		if (objectManagementDeleteTitle != nullptr)
		{
			objectManagementDeleteTitle->Disable();
		}

		if (objectManagementDeleteNameTitle != nullptr)
		{
			objectManagementDeleteNameTitle->Disable();
		}

		if (objectManagementDeleteName != nullptr)
		{
			objectManagementDeleteName->Disable();
		}

		if (objectManagementBackground != nullptr)
		{
			objectManagementBackground->Disable();
		}

		if (objectManagementTitle != nullptr)
		{
			objectManagementTitle->Disable();
		}

		if (objectManagementCreateTitle != nullptr)
		{
			objectManagementCreateTitle->Disable();
		}

		if (objectManagementCreateObjectName != nullptr)
		{
			objectManagementCreateObjectName->Disable();
		}

		if (objectManagementCreateObjectNameInput != nullptr)
		{
			objectManagementCreateObjectNameInput->Disable();
		}

		if (objectManagementCreateObjectParentSceneName != nullptr)
		{
			objectManagementCreateObjectParentSceneName->Disable();
		}

		if (objectManagementCreateObjectParentSceneNameInput != nullptr)
		{
			objectManagementCreateObjectParentSceneNameInput->Disable();
		}

		if (objectManagementSelectedObject != nullptr)
		{
			objectManagementSelectedObject->Disable();
		}
	}

	objectManagementInterfaceEnabled = false;
}

void EditorUI::Disable()
{
	disabled = true;

	if (playButton != nullptr)
	{
		playButton->Disable();
	}

	DisableSceneManagementInterface();
	CleanupInterfaceToggleCallbacks();
	DisableObjectManagementInterface();
}

void EditorUI::Enable()
{
	CreateInterfaceToggleCallbacks();

	if (sceneManagementInterfaceEnabled)
	{
		EnableSceneManagementInterface();
	}

	if (playButton != nullptr)
	{
		playButton->Enable();
	}

	disabled = false;
}
