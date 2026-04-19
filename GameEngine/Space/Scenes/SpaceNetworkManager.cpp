#include "SpaceNetworkManager.h"

#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"

SpaceNetworkManager::SpaceNetworkManager() :
	receiveLoginInfo(nullptr),
	receiveLoginServerResponse(nullptr)
{
}

SpaceNetworkManager::~SpaceNetworkManager()
{
}

void SpaceNetworkManager::Initialize()
{
	receiveLoginInfo = new std::function<void(const std::string&)>([](const std::string& data)
		{
			// The IP address is always first and ends with a space.
			// The function ID is next followed by a space.
			// Then there is the data block formatted in any way but since it is a string no '\0' characters are allowed.

			std::string ID = NetworkManager::GetIDFromData(data);

			std::string functionID = NetworkManager::GetFunctionFromData(data);

			std::string response = "Login Success";

			NetworkManager::ServerSend(ID, response, "SpaceNetworkManagerClientLogin");
		});

	NetworkManager::RegisterReceiveDataFunction("SpaceNetworkManagerServerLogin", receiveLoginInfo);

	receiveLoginServerResponse = new std::function<void(const std::string&)>([](const std::string& data)
		{
			if (NetworkManager::GetDataBlockFromData(data) == "Login Success")
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
		});

	NetworkManager::RegisterReceiveDataFunction("SpaceNetworkManagerClientLogin", receiveLoginServerResponse);

	Scene::Initialize();
}

void SpaceNetworkManager::Terminate()
{
	Scene::Terminate();

	NetworkManager::DeregisterReceiveDataFunction("SpaceNetworkManagerServerLogin");
	delete receiveLoginInfo;

	NetworkManager::DeregisterReceiveDataFunction("SpaceNetworkManagerClientLogin");
	delete receiveLoginServerResponse;
}

void SpaceNetworkManager::Load()
{
	Scene::Load();
}

void SpaceNetworkManager::Unload()
{
	Scene::Unload();
}

void SpaceNetworkManager::Start()
{
	Scene::Start();
}

void SpaceNetworkManager::End()
{
	Scene::End();
}
