#include "JoinUI.h"

#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/UI/Button.h"
#include "GameEngine/UI/TextField.h"

JoinUI::JoinUI() :
	GameObject("JoinUI")
{
	RegisterGameObjectClassType<JoinUI>(this);
}

JoinUI::~JoinUI()
{
}

void JoinUI::Initialize()
{
	buttonFunc = new std::function<void()>([this]()
		{
			NetworkManager::ClientSend("", "FPSNetworkManagerServerLogin");
		});

	button = new Button("StartIdle", "Start", "StartPress", "Start", { 0.5f, 0.5f }, buttonFunc);
	button->SetScale(button->GetScale() / 2.0f);

	text = new TextField("Press The Button To Join", "exo2", glm::vec2(0.5f, 0.5f), glm::vec2(25.0f, 25.0f));
}

void JoinUI::Terminate()
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

void JoinUI::GameUpdate()
{
	if (button != nullptr)
	{
		button->Update();
	}
}

void JoinUI::EditorUpdate()
{
}

void JoinUI::Load()
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

void JoinUI::Unload()
{
}

void JoinUI::Start()
{
	if (!SceneManager::SceneLoaded("FPSNetworkManager"))
	{
		SceneManager::LoadScene("FPSNetworkManager");
		SceneManager::InitializeScene("FPSNetworkManager");
		SceneManager::StartScene("FPSNetworkManager");
	}

	if (!NetworkManager::IsServer())
	{

	}
	else
	{
		std::string sceneName = "Test";

		if (!SceneManager::SceneLoaded(sceneName))
		{
			if (!SceneManager::SceneLoaded(sceneName))
			{
				SceneManager::LoadScene(sceneName);
				SceneManager::InitializeScene(sceneName);
			}

			SceneManager::GetRegisteredScene(sceneName)->Deserialize("Assets/Scenes/FPS.xml");
			SceneManager::StartScene(sceneName);
		}
	}
}
