#ifndef NETWORKOBJECT_H
#define NETWORKOBJECT_H

#include "../Utils/Logger.h"

#include <string>
#include <functional>
#include <unordered_set>

class GameObject;

class NetworkObject
{

public:

	virtual void OnSpawn();

	virtual void OnDespawn();

	virtual void OnDataReceived(const std::string& Data);

	unsigned long long GetNetworkObjectID() const;

	void ClientSend(const std::string& data);

	void ServerSend(const std::string& IP, const std::string& data);

	void ServerSendAll(const std::string& data, const std::unordered_set<std::string>& excludedIPs = {});

	bool SpawnedFromLocalSpawnRequest() const;

protected:

	NetworkObject();

	virtual ~NetworkObject();

	template<typename T>
	void RegisterNetworkObjectClassType(NetworkObject* obj);

private:

	friend class NetworkManager;

	NetworkObject(const NetworkObject&) = delete;

	NetworkObject& operator=(const NetworkObject&) = delete;

	NetworkObject(NetworkObject&&) = delete;

	NetworkObject& operator=(NetworkObject&&) = delete;

	static std::function<void(NetworkObject**)> GetConstructor(const std::string& name);

	static std::unordered_map<std::string, std::function<void(NetworkObject**)>> newFunctions;

	std::function<void(const std::string&)>* onClientRecieveID;

	std::function<void(const std::string&)>* onServerReceiveIDRequest;

	unsigned long long networkObjectID;

	std::string nameOfType;

	std::function<void(const std::string&)>* onReceiveData;

	bool spawnedFromLocalSpwanRequest;
};

template<typename T>
inline void NetworkObject::RegisterNetworkObjectClassType(NetworkObject* obj)
{
	nameOfType = std::string(typeid(*obj).name());
	nameOfType.replace(0, 6, "");

	if (newFunctions.find(nameOfType) == newFunctions.end())
	{
		Logger::Log(std::string("Registered NetworkObject of Type: ") + nameOfType, Logger::Category::Info);

		newFunctions[nameOfType] = std::function<void(NetworkObject**)>([](NetworkObject** outNewGameObject)
			{
				*outNewGameObject = new T();
			});
	}
}

#endif // NETWORKOBJECT_H