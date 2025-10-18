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

EditorUI::EditorUI() :
	disabled(false),
	sceneManagementInterfaceEnabled(true),
	altPress(nullptr),
	sPress(nullptr)
{
	CreatePlayButton();
	CreateSceneManagementInterface();
	CreateInterfaceToggleCallbacks();
}

EditorUI::~EditorUI()
{
	CleanupInterfaceToggleCallbacks();
	CleanupSceneManagementInterface();
	delete playButton;
}

void EditorUI::Update()
{
	playButton->Update();
	UpdateSceneManagementInterface();
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
	ModelManager::LoadModel("Play", "Assets/Model/PlayButton.gltf");

	static std::function<void()> buttonFunc = std::function<void()>([this]() 
		{
			Editor::Disable();
		});

	playButton = new Button("PlayIdle", "Play", "PlayPress", "Play", { 0.5f, 0.9f }, &buttonFunc);
}

void EditorUI::CreateLoadSceneInputFields()
{
	TextureManager::LoadTexture("Assets/Texture/UIBackground0.png", "LoadSceneInputFieldBackground");
	TextureManager::LoadTexture("Assets/Texture/UIBackground0.png", "LoadSceneInputFieldBackgroundHover");
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

			Scene* scene = SceneManager::GetLoadedScene(sceneName);

			if (scene != nullptr)
			{
				SceneManager::UnloadScene(sceneName);
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

			Scene* scene = SceneManager::GetLoadedScene(sceneName);

			if (scene != nullptr)
			{
				scene->Save(fileName);
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
}

void EditorUI::CleanupInterfaceToggleCallbacks()
{
	InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_S, "EditorUI");

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

}

void EditorUI::Disable()
{
	disabled = true;
	playButton->Disable();
	DisableSceneManagementInterface();
	CleanupInterfaceToggleCallbacks();
}

void EditorUI::Enable()
{
	CreateInterfaceToggleCallbacks();

	if (sceneManagementInterfaceEnabled)
	{
		EnableSceneManagementInterface();
	}

	playButton->Enable();
	disabled = false;
}
