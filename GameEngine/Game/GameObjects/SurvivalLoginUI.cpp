#include "SurvivalLoginUI.h"

#include "GameEngine/UI/InputField.h"
#include "GameEngine/Renderer/Text/Text.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"

SurvivalLoginUI::SurvivalLoginUI() :
	GameObject("SurvivalLoginUI"),
	username(nullptr),
	password(nullptr)
{
	RegisterGameObjectClassType<SurvivalLoginUI>(this);
}

SurvivalLoginUI::~SurvivalLoginUI()
{
}

void SurvivalLoginUI::Initialize()
{
	username = new InputField("UsernameField", "UsernameFieldHover", "UsernameFieldPress", { 0.15625, 0.037037037037037 }, { 0.5, 0.5 });
	password = new InputField("UsernameField", "UsernameFieldHover", "UsernameFieldPress", { 0.15625, 0.037037037037037 }, { 0.5, 0.4 });

	passwordOnEnter = new std::function<void()>([this]()
		{
			std::string user = username->GetText();
			std::string pass = password->GetText();

			std::string data = user + pass;

			NetworkManager::ClientSend(data, "SurvivalNetworkManagerServerLogin");
		});

	password->SetOnEnter(passwordOnEnter);

}

void SurvivalLoginUI::Terminate()
{
	if (passwordOnEnter != nullptr)
	{
		delete passwordOnEnter;
		passwordOnEnter = nullptr;
	}

	if (username != nullptr)
	{
		delete username;
		username = nullptr;
	}

	if (password != nullptr)
	{
		delete password;
		password = nullptr;
	}
}

void SurvivalLoginUI::GameUpdate()
{
	if (username != nullptr)
	{
		username->Update();
	}

	if (password != nullptr)
	{
		password->Update();
	}
}

void SurvivalLoginUI::EditorUpdate()
{
}

void SurvivalLoginUI::Load()
{
	if (!TextureManager::TextureLoaded("UsernameField"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "UsernameField");
	}

	if (!TextureManager::TextureLoaded("UsernameFieldPress"))
	{
		TextureManager::LoadTexture("Assets/Texture/Green.png", "UsernameFieldPress");
	}

	if (!TextureManager::TextureLoaded("UsernameFieldHover"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "UsernameFieldHover");
	}
}

void SurvivalLoginUI::Unload()
{
	if (TextureManager::TextureLoaded("UsernameField"))
	{
		TextureManager::UnloadTexture("UsernameField");
	}

	if (TextureManager::TextureLoaded("UsernameFieldPress"))
	{
		TextureManager::UnloadTexture("UsernameFieldPress");
	}

	if (TextureManager::TextureLoaded("UsernameFieldHover"))
	{
		TextureManager::UnloadTexture("UsernameFieldHover");
	}
}

void SurvivalLoginUI::Start()
{
	if (!SceneManager::SceneLoaded("SurvivalNetworkManager"))
	{
		SceneManager::LoadScene("SurvivalNetworkManager");
		SceneManager::InitializeScene("SurvivalNetworkManager");
		SceneManager::StartScene("SurvivalNetworkManager");
	}

	if (!NetworkManager::IsServer())
	{
		
	}
	else
	{
		std::string sceneName = "SurvivalScene";

		if (!SceneManager::SceneLoaded(sceneName))
		{
			if (!SceneManager::SceneLoaded(sceneName))
			{
				SceneManager::LoadScene(sceneName);
			}

			SceneManager::GetRegisteredScene(sceneName)->Deserialize("Assets/Scenes/Main.xml");
			SceneManager::StartScene(sceneName);
		}
	}
}
