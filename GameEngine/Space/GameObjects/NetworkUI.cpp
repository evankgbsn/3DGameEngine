#include "NetworkUI.h"

#include "GameEngine/UI/TextField.h"
#include "GameEngine/UI/Button.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"

NetworkUI::NetworkUI() : 
	GameObject("NetworkUI"),
	text(nullptr),
	button(nullptr),
	buttonFunc(nullptr)
{
	RegisterGameObjectClassType<NetworkUI>(this);
}

NetworkUI::~NetworkUI()
{
}

void NetworkUI::Initialize()
{
	buttonFunc = new std::function<void()>([this]()
		{
			NetworkManager::ClientSend("", "SpaceNetworkManagerServerLogin");
		});

	button = new Button("StartIdle", "Start", "StartPress", "Start", { 0.5f, 0.5f }, buttonFunc);
	button->SetScale(button->GetScale() / 2.0f);

	text = new TextField("Press The Button To Join", "arial", glm::vec2(0.33f, 0.6f), glm::vec2(4.0f, 4.0f), { 1.0f, 1.0f, 1.0f, 1.0f });

	Scene* asteroidField = SceneManager::GetRegisteredScene("AsteroidField");

	if (asteroidField != nullptr)
	{
		static std::function<void(const std::string&)> loadedCallback([this](const std::string& loadedObject)
			{
				text->SetText(std::string("Loading: ") + loadedObject);
				text->SetPosition(glm::vec2(0.42f, 0.6f));
			});

		asteroidField->RegisterObjectLoadedCallback(&loadedCallback);
	}
}

void NetworkUI::Terminate()
{
	if (text != nullptr)
	{
		delete text;
		text = nullptr;
	}

	if (button != nullptr)
	{
		delete button;
		button = nullptr;
	}

	if (buttonFunc != nullptr)
	{
		delete buttonFunc;
		buttonFunc = nullptr;
	}
}

void NetworkUI::GameUpdate()
{
	if (button != nullptr)
	{
		button->Update();
	}
}

void NetworkUI::EditorUpdate()
{
}

void NetworkUI::Load()
{
	if (!ModelManager::ModelLoaded("Start"))
	{
		ModelManager::LoadModel("Start", "Assets/Model/PlayButton.gltf", false);
	}

	if (!TextureManager::TextureLoaded("Start"))
	{
		TextureManager::LoadTexture("Assets/Texture/Green.png", "Start");
	}

	if (!TextureManager::TextureLoaded("StartPress"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "StartPress");
	}

	if (!TextureManager::TextureLoaded("StartIdle"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "StartIdle");
	}
}

void NetworkUI::Unload()
{
}

void NetworkUI::Start()
{
	if (!SceneManager::SceneLoaded("SpaceNetworkManager"))
	{
		SceneManager::LoadScene("SpaceNetworkManager");
		SceneManager::InitializeScene("SpaceNetworkManager");
		SceneManager::StartScene("SpaceNetworkManager");
	}

	if (!NetworkManager::IsServer())
	{

	}
	else
	{
		std::string sceneName = "AsteroidField";

		if (!SceneManager::SceneLoaded(sceneName))
		{
			SceneManager::LoadScene(sceneName);
			SceneManager::InitializeScene(sceneName);

			SceneManager::GetRegisteredScene(sceneName)->Deserialize("Assets/Scenes/AsteroidField.xml");
			SceneManager::StartScene(sceneName);
		}
	}
}
