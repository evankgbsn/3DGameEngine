#include "FPSNetworkManager.h"

#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"


FPSNetworkManager::FPSNetworkManager()
{
}

FPSNetworkManager::~FPSNetworkManager()
{
}

void FPSNetworkManager::Initialize()
{
	receiveLoginInfo = new std::function<void(const std::string&)>([](const std::string& data)
		{
			// The IP address is always first and ends with a space.
			// The function ID is next followed by a space.
			// Then there is the data block formatted in any way but since it is a string no '\0' characters are allowed.

			std::string IP = NetworkManager::GetIPFromData(data);

			std::string functionID = NetworkManager::GetFunctionFromData(data);

			std::string response = "Login Success";

			NetworkManager::ServerSend(IP, response, "FPSNetworkManagerClientLogin");
		});

	NetworkManager::RegisterReceiveDataFunction("FPSNetworkManagerServerLogin", receiveLoginInfo);

	receiveLoginServerResponse = new std::function<void(const std::string&)>([](const std::string& data)
		{
			if (NetworkManager::GetDataBlockFromData(data) == "Login Success")
			{
				std::string sceneName = "Test";

				if (!SceneManager::SceneLoaded(sceneName))
				{
					SceneManager::LoadScene(sceneName);
					SceneManager::InitializeScene(sceneName);
					SceneManager::GetRegisteredScene(sceneName)->Deserialize("Assets/Scenes/FPS.xml");
					SceneManager::StartScene(sceneName);
				}
			}
		});

	NetworkManager::RegisterReceiveDataFunction("FPSNetworkManagerClientLogin", receiveLoginServerResponse);


	Scene::Initialize();
}

void FPSNetworkManager::Terminate()
{
	Scene::Terminate();

	NetworkManager::DeregisterReceiveDataFunction("FPSNetworkManagerServerLogin");
	delete receiveLoginInfo;

	NetworkManager::DeregisterReceiveDataFunction("FPSNetworkManagerClientLogin");
	delete receiveLoginServerResponse;
}

void FPSNetworkManager::Load()
{
}

void FPSNetworkManager::Unload()
{
}
