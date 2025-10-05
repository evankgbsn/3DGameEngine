#include "NetworkManager.h"

#include "../Utils/SingletonHelpers.h"

#define DEFAULT_PORT "27015"
#define SERVER_IP "136.30.15.215"
#define DEFAULT_BUFFLEN 512

NetworkManager* NetworkManager::instance = nullptr;

void NetworkManager::Initialize()
{
	SingletonHelpers::InitializeSingleton<NetworkManager>(&instance, "NetworkManager");
}

void NetworkManager::Terminate()
{
	SingletonHelpers::TerminateSingleton<NetworkManager>(&instance, "NetworkManager");
}

void NetworkManager::Start(bool isServer)
{
	if (instance != nullptr)
	{
		if (instance->started)
		{
			return;
		}

		instance->running.store(true);

		instance->isServer = isServer;

		if (!instance->isServer)
		{
			instance->StartClient();
		}
		else
		{
			instance->StartServer();
		}

		instance->started = true;
	}
}

void NetworkManager::Update()
{
	
}

void NetworkManager::EditorUpdate()
{
	
}

bool NetworkManager::IsServer()
{
	if (instance != nullptr)
	{
		return instance->isServer;
	}

	return false;
}

NetworkManager::NetworkManager() :
	connectSocket(INVALID_SOCKET),
	connectedClients(),
	isServer(false),
	started(false)
{
	InitializeWinsock();
}

NetworkManager::~NetworkManager()
{
	running.store(false);

	if (!isServer)
	{
		closesocket(connectSocket);
		receiveThread.join();
	}
	else
	{
		closesocket(listenSocket);
		for (auto& client : connectedClients)
		{
			closesocket(client.second);
		}
		listenThread.join();
		receiveThread.join();
	}

	WSACleanup();
}

void NetworkManager::InitializeWinsock()
{
	WSADATA wsaData;

	int result;

	// Initialize winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		Logger::Log("WSAStartup failed: " + std::to_string(result), Logger::Category::Error);
		return;
	}
}

void NetworkManager::StartClient()
{
	addrinfo* result = nullptr;
	addrinfo* ptr = nullptr;
	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int res = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
	if (res != 0) {
		Logger::Log("getaddrinfo failed: " + std::to_string(res), Logger::Category::Error);
		return;
	}

	// Create a SOCKET for connecting to server
	connectSocket = INVALID_SOCKET;
	
	ptr = result;

	connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (connectSocket == INVALID_SOCKET) {
		Logger::Log("Error at socket(): " + std::to_string(WSAGetLastError()), Logger::Category::Error);
		freeaddrinfo(result);
		return;
	}

	// Connect to server.
	res = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (res == SOCKET_ERROR) {
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET) {
		Logger::Log("Unable to connect to server!", Logger::Category::Error);
		return;
	}

	int recvbuflen = DEFAULT_BUFFLEN;
	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFFLEN];

	// Send an initial buffer
	res = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (res == SOCKET_ERROR) {
		Logger::Log("send failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
		closesocket(connectSocket);
		return;
	}

	Logger::Log("Bytes Sent: " + std::to_string(res), Logger::Category::Success);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	res = shutdown(connectSocket, SD_SEND);
	if (res == SOCKET_ERROR) {
		Logger::Log("shutdown failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
		closesocket(connectSocket);
		return;
	}

	receiveThread = std::thread(&NetworkManager::Receive, this);

}

void NetworkManager::StartServer()
{
	addrinfo* result = nullptr;
	addrinfo* ptr = nullptr;
	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int res = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);

	if (res != 0)
	{
		Logger::Log("getaddrinfo failed: " + std::to_string(res), Logger::Category::Error);
		return;
	}

	// Create a socket for the server to listen for client connections
	listenSocket = INVALID_SOCKET;

	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET)
	{
		Logger::Log("Error at socket(): " + std::to_string(WSAGetLastError()), Logger::Category::Error);
		freeaddrinfo(result);
		return;
	}

	// Setup the TCP listening socket
	res = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (res == SOCKET_ERROR)
	{
		Logger::Log("bind failed with error: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
		freeaddrinfo(result);
		closesocket(listenSocket);
		return;
	}

	freeaddrinfo(result);

	listenThread = std::thread(&NetworkManager::ListenForConnections, this);
	receiveThread = std::thread(&NetworkManager::Receive, this);
}

void NetworkManager::Receive()
{
	int recvbuflen = DEFAULT_BUFFLEN;
	char recvbuf[DEFAULT_BUFFLEN] = {};

	while (running.load())
	{
		if (!isServer)
		{
			int iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				Logger::Log("Bytes received: " + std::to_string(iResult), Logger::Category::Success);

				std::lock_guard<std::mutex> guard(receivedDataMutex);
				receivedData.push_back(recvbuf);
			}
			else if (iResult < 0)
			{
				Logger::Log("recv failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
			}
		}
		else
		{
			if (!connectedClients.empty())
			{
				std::lock_guard<std::mutex> lockGuard(connectedClientsMutex);
				for (auto& clientSocket : connectedClients)
				{
					if (clientSocket.second != INVALID_SOCKET)
					{
						int iResult = recv(clientSocket.second, recvbuf, recvbuflen, 0);
						if (iResult > 0) 
						{
							printf("Bytes received: %d\n", iResult);
						}
						else if(iResult < 0)
						{
							Logger::Log("recv failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
							closesocket(clientSocket.second);
							return;
						}
					}
				}
			}
		}
	}
}

void NetworkManager::ListenForConnections()
{
	while (running.load())
	{
		// Listen for incomming connections
		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			Logger::Log("Listen failed with error: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
			closesocket(listenSocket);
			return;
		}

		// Accept a client connection
		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
		if (clientSocket == INVALID_SOCKET)
		{
			Logger::Log("accept failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
			closesocket(listenSocket);
			return;
		}

		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);


		std::lock_guard<std::mutex> lockGuard(connectedClientsMutex);
		connectedClients[clientIP] = clientSocket;
	}
}

void NetworkManager::ServerSendAll(const std::string& data)
{
	if (instance != nullptr)
	{
		std::lock_guard<std::mutex> guard(instance->connectedClientsMutex);
		for (auto& clientSocket : instance->connectedClients)
		{
			int iSendResult = send(clientSocket.second, data.data(), data.size(), 0);
			if (iSendResult == SOCKET_ERROR) {
				Logger::Log("send failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
				closesocket(clientSocket.second);
				return;
			}
			Logger::Log("Bytes sent: " + std::to_string(iSendResult) + " to: " + clientSocket.first, Logger::Category::Success);
		}
	}
}
