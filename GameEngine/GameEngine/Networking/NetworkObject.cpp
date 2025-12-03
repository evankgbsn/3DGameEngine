#include "NetworkObject.h"

#include "NetworkManager.h"

std::unordered_map<std::string, std::function<void(NetworkObject**)>> NetworkObject::newFunctions = std::unordered_map<std::string, std::function<void(NetworkObject**)>>();

void NetworkObject::OnServerSpawnConfirmation(const std::string& IP)
{
	serverConfirmedIPs.insert(IP);
}

void NetworkObject::OnClientSpawnConfirmation()
{
}

void NetworkObject::OnSpawn()
{
	NetworkManager::RegisterReceiveDataFunction("NetworkObject:" + std::to_string(networkObjectID), onReceiveData);
	NetworkManager::RegisterReceiveDataFunction("NetworkObjectUDP:" + std::to_string(networkObjectID), onReceiveDataUDP);
}

void NetworkObject::OnDespawn()
{
	NetworkManager::DeregisterReceiveDataFunction("NetworkObjectUDP:" + std::to_string(networkObjectID));
	NetworkManager::DeregisterReceiveDataFunction("NetworkObject:" + std::to_string(networkObjectID));
}

void NetworkObject::ClientSend(const std::string& data, bool reliable)
{
	if (reliable)
	{
		NetworkManager::ClientSend(data, "NetworkObject:" + std::to_string(networkObjectID));
	}
	else
	{
		NetworkManager::ClientSendUDP(data, "NetworkObjectUDP:" + std::to_string(networkObjectID));
	}
}

void NetworkObject::ServerSend(const std::string& IP, const std::string& data, bool reliable)
{
	if (serverConfirmedIPs.contains(IP))
	{
		if (reliable)
		{
			NetworkManager::ServerSend(IP, data, "NetworkObject:" + std::to_string(networkObjectID));
		}
		else
		{
			NetworkManager::ServerSendUDP(IP, data, "NetworkObjectUDP:" + std::to_string(networkObjectID));
		}
	}
}

void NetworkObject::ServerSendAll(const std::string& data, const std::unordered_set<std::string>& excludedIPs, bool reliable)
{
	for (const std::string& ip : serverConfirmedIPs)
	{
		if (!excludedIPs.contains(ip))
		{
			ServerSend(ip, data, reliable);
		}
	}
}

bool NetworkObject::SpawnedFromLocalSpawnRequest() const
{
	return spawnedFromLocalSpawnRequest;
}

std::string NetworkObject::GetSpawnerIP() const
{
	return spawnerIP;
}

void NetworkObject::OnDataReceived(const std::string& Data)
{
}

unsigned long long NetworkObject::GetNetworkObjectID() const
{
	return networkObjectID;
}

std::function<void(NetworkObject**)> NetworkObject::GetConstructor(const std::string& name)
{
	if (newFunctions.find(name) != newFunctions.end())
	{
		return newFunctions[name];
	}

	return std::function<void(NetworkObject**)>();
}

NetworkObject::NetworkObject() :
	spawnedFromLocalSpawnRequest(false)
{
	onReceiveData = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			OnDataReceived(NetworkManager::GetDataBlockFromData(data));
		});

	onReceiveDataUDP = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			OnDataReceived(NetworkManager::GetDataBlockFromData(data));
		});
}

NetworkObject::~NetworkObject()
{
	delete onReceiveDataUDP;
	delete onReceiveData;
}
