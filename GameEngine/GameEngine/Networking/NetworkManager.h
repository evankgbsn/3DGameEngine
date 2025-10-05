#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <thread>
#include <unordered_map>
#include <mutex>
#include <list>

class SingletonHelpers;


class NetworkManager
{
public:

	static void Initialize();

	static void Terminate();

	static void Start(bool isServer);

	static void Update();

	static void EditorUpdate();

	static bool IsServer();

	static void ServerSendAll(const std::string& data);

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

	void Receive();

	void ListenForConnections();

	static NetworkManager* instance;

	bool isServer;

	bool started;

	SOCKET connectSocket;

	SOCKET listenSocket;

	std::thread listenThread;

	std::thread receiveThread;

	std::mutex connectedClientsMutex;

	std::unordered_map<std::string, SOCKET> connectedClients;

	std::mutex receivedDataMutex;

	std::list<std::string> receivedData;

	std::atomic<bool> running;
	
};

#endif // NETWORKMANAGER_H