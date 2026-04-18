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

	virtual void OnServerSpawnConfirmation(const std::string& IP);

	virtual void OnClientSpawnConfirmation();

	virtual void OnSpawn();

	virtual void OnDespawn();

	virtual void OnDataReceived(const std::string& Data);

	unsigned long long GetNetworkObjectID() const;

	void ClientSend(const std::string& data, bool reliable = true);

	void ServerSend(const std::string& IP, const std::string& data, bool reliable = true);

	void ServerSendAll(const std::string& data, const std::unordered_set<std::string>& excludedIPs = {}, bool reliable = true);

	bool SpawnedFromLocalSpawnRequest() const;

	std::string GetSpawnerID() const;

	void AddServerDataReceivedCallback(const std::string& type, std::function<void(const std::string&)>*);

	void RemoveServerDataReceivedCallback(const std::string& type);

	void AddClientDataReceivedCallback(const std::string& type, std::function<void(const std::string&)>*);

	void RemoveClientDataReceivedCallback(const std::string& type);

	bool IsServer() const;

	bool IsClient() const;

	bool IsLocalClient() const;

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

	void ProcessReceivedData(const std::string& data, std::string& updateType, std::string& packetID, std::string& updateData);

	static std::function<void(NetworkObject**)> GetConstructor(const std::string& name);

	static std::unordered_map<std::string, std::function<void(NetworkObject**)>> newFunctions;

	unsigned long long networkObjectID;

	std::string nameOfType;

	std::function<void(const std::string&)>* onReceiveData;

	std::function<void(const std::string&)>* onReceiveDataUDP;

	bool spawnedFromLocalSpawnRequest;

	std::string spawnerID;

	std::unordered_set<std::string> serverConfirmedIDs;

	std::unordered_map<std::string, std::function<void(const std::string&)>*> serverDataReceivedFunctions;

	std::unordered_map<std::string, std::function<void(const std::string&)>*> clientDataReceivedFunctions;

	std::unordered_map<std::string, unsigned long long> serverLastPacketIDs;

	std::unordered_map<std::string, unsigned long long> clientLastPacketIDs;

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