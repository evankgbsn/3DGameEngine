#include "SurvivalNetworkManager.h"

#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"

SurvivalNetworkManager::SurvivalNetworkManager()
{
}

SurvivalNetworkManager::~SurvivalNetworkManager()
{
}

void SurvivalNetworkManager::Initialize()
{
	receiveLoginInfo = new std::function<void(const std::string&)>([](const std::string& data)
		{
			// The IP address is always first and ends with a space.
			// The function ID is next followed by a space.
			// Then there is the data block formatted in any way but since it is a string no '\0' characters are allowed.

			std::string IP = NetworkManager::GetIPFromData(data);

			std::string functionID = NetworkManager::GetFunctionFromData(data);

			std::string response = "Login Success";

			NetworkManager::ServerSend(IP, response, "SurvivalNetworkManagerClientLogin");
		});

	NetworkManager::RegisterReceiveDataFunction("SurvivalNetworkManagerServerLogin", receiveLoginInfo);

	receiveLoginServerResponse = new std::function<void(const std::string&)>([](const std::string& data)
		{
			if (NetworkManager::GetDataBlockFromData(data) == "Login Success")
			{
				if (!SceneManager::SceneLoaded("SurvivalScene"))
				{
					SceneManager::LoadScene("SurvivalScene");
					SceneManager::InitializeScene("SurvivalScene");
					SceneManager::StartScene("SurvivalScene");
				}
			}
		});

	NetworkManager::RegisterReceiveDataFunction("SurvivalNetworkManagerClientLogin", receiveLoginServerResponse);
	

	Scene::Initialize();
}

void SurvivalNetworkManager::Terminate()
{
	Scene::Terminate();

	NetworkManager::DeregisterReceiveDataFunction("SurvivalNetworkManagerServerLogin");
	delete receiveLoginInfo;

	NetworkManager::DeregisterReceiveDataFunction("SurvivalNetworkManagerClientLogin");
	delete receiveLoginServerResponse;
}
