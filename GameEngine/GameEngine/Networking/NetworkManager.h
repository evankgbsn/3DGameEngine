#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <glm/glm.hpp>

#include <thread>
#include <unordered_map>
#include <mutex>
#include <list>
#include <functional>
#include <string>
#include <unordered_set>

class SingletonHelpers;
class NetworkObject;

class NetworkManager
{
public:

	static void Initialize();

	static void Terminate();

	static void Start(bool isServer);

	static void Update();

	static void EditorUpdate();

	static bool IsServer();

	static void ServerSendAll(const std::string& data, const std::string& receiveFunction, const std::unordered_set<std::string>& excludedIPs = {});

	static void ServerSend(const std::string& ip, const std::string& data, const std::string& receiveFunction);

	static void ClientSend(const std::string& data, const std::string& receiveFunction);

	static void RegisterReceiveDataFunction(const std::string& key, std::function<void(const std::string&)>*);

	static void DeregisterReceiveDataFunction(const std::string& key);

	static void RegisterOnClientDisconnectFunction(const std::string& key, std::function<void(const std::string&)>* callback);

	static void DeregisterOnClientDisconnectFunction(const std::string& key);

	static void RegisterOnDisconnectFunction(const std::string& key, std::function<void()>* callback);

	static void DeregisterOnDisconnectFunction(const std::string& key);

	static std::string GetIPFromData(const std::string& data);

	static std::string GetFunctionFromData(const std::string& data);

	static std::string GetDataBlockFromData(const std::string data);

	static std::string ConvertVec3ToData(const glm::vec3& vec3);

	static glm::vec3 ConvertDataToVec3(const std::string& data);

	static std::string GetIP();

	static unsigned long long GenerateNetworkObjectID();

	static void Spawn(const std::string& networkObjectClassName, std::function<void(NetworkObject*)>* callback);

	static void Despawn(unsigned long long networkObjectID);

private:

	friend class SingletonHelpers;

	NetworkManager();

	~NetworkManager();

	NetworkManager(const NetworkManager&) = delete;

	NetworkManager& operator=(const NetworkManager&) = delete;

	NetworkManager(NetworkManager&&) = delete;

	NetworkManager& operator=(NetworkManager&&) = delete;

	void InitializeWinsock();

	void StartClient();

	void StartServer();

	void ClientReceive();

	void ServerReceive(const std::string& IP);

	void ListenForConnections();

	void ProcessReceivedData();

	void SetupServerReceiveSpawnRequestCallback();

	void CleanupServerReceiveSpawnRequestCallback();

	void SetupServerReceiveDespawnRequestCallback();

	void CleanupServerReceiveDespawnRequestCallback();
	
	void CleanupSpawnedNetworkObjects();

	void SetupReceiveSpawnFromServer();

	void CleanupReceiveSpawnFromServer();

	void SetupReceiveDespawnFromServer();

	void CleanupReceiveDespawnFromServer();

	void OnClientDisconnect(const std::string& IP);

	void CleanDisconnectedClientThreads();

	void ProcessMainThreadUpdates();

	static NetworkManager* instance;

	std::string clientIP;

	bool isServer;

	bool started;

	SOCKET connectSocket;

	SOCKET listenSocket;

	std::thread listenThread;

	std::thread receiveThread;

	std::mutex connectedClientsMutex;

	std::unordered_map<std::string, SOCKET> connectedClients;

	std::mutex connectedClientsReceiveThreadsMutex;

	std::unordered_map<std::string, std::thread> connectedClientsReceiveThreads;

	std::mutex receivedDataMutex;

	std::list<std::string> receivedData;

	std::atomic<bool> running;

	std::unordered_map<std::string, std::function<void(const std::string&)>*> responseFunctions;

	std::unordered_map<unsigned long long, std::function<void(const std::string&)>*> clientSpawnRequestCallbacks;

	std::function<void(const std::string&)>* serverReceiveSpawnRequest;

	std::function<void(const std::string&)>* serverReceiveDespawnRequest;

	std::unordered_map<unsigned long long, NetworkObject*> spawnedNetworkObjects;

	std::function<void(const std::string&)>* receiveSpawnFromServer;

	std::function<void(const std::string&)>* receiveDespawnFromServer;

	std::mutex onClientDisconnectCallbacksMutex;

	std::unordered_map<std::string, std::function<void(const std::string&)>*> onClientDisconnectCallbacks;

	std::unordered_map<std::string, std::function<void()>*> onDisconnectCallbacks;

	std::mutex mainThreadUpdatesMutex;

	std::list<std::function<void()>> mainThreadUpdates;

	std::thread cleanDisconnectedClientThread;

	unsigned long long networkObjectIDGenerator;
	
};

#endif // NETWORKMANAGER_H