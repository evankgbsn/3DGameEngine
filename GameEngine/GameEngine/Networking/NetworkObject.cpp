#include "NetworkObject.h"

#include "NetworkManager.h"

std::unordered_map<std::string, std::function<void(NetworkObject**)>> NetworkObject::newFunctions = std::unordered_map<std::string, std::function<void(NetworkObject**)>>();

void NetworkObject::OnSpawn()
{
	NetworkManager::RegisterReceiveDataFunction("NetworkObject:" + networkObjectID, onReceiveData);
}

void NetworkObject::OnDespawn()
{
	NetworkManager::DeregisterReceiveDataFunction("NetworkObject:" + networkObjectID);
}

void NetworkObject::ClientSend(const std::string& data)
{
	NetworkManager::ClientSend(data, "NetworkObject:" + networkObjectID);
}

void NetworkObject::ServerSend(const std::string& IP, const std::string& data)
{
	NetworkManager::ServerSend(IP, data, "NetworkObject:" + networkObjectID);
}

void NetworkObject::ServerSendAll(const std::string& data, const std::unordered_set<std::string>& excludedIPs)
{
	NetworkManager::ServerSendAll(data, "NetworkObject:" + networkObjectID, excludedIPs);
}

bool NetworkObject::SpawnedFromLocalSpawnRequest() const
{
	return spawnedFromLocalSpawnRequest;
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
}

NetworkObject::~NetworkObject()
{
	delete onReceiveData;
}
