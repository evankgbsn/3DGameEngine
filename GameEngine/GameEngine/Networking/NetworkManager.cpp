#include "NetworkManager.h"

#include "../Utils/SingletonHelpers.h"
#include "NetworkObject.h"

#define DEFAULT_PORT "27015"
//#define SERVER_IP "136.30.15.215"
#define SERVER_IP "192.168.50.2"

#define DEFAULT_BUFFLEN 512

NetworkManager* NetworkManager::instance = nullptr;

void NetworkManager::Initialize()
{
	SingletonHelpers::InitializeSingleton<NetworkManager>(&instance, "NetworkManager");

	instance->SetupServerReceiveSpawnRequestCallback();
	instance->SetupReceiveSpawnFromServer();
}

void NetworkManager::Terminate()
{
	instance->CleanupReceiveSpawnFromServer();
	instance->CleanupSpawnedNetworkObjects();
	instance->CleanupServerReceiveSpawnRequestCallback();

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
	if (instance != nullptr)
	{
		instance->ProcessReceivedData();
	}
}

void NetworkManager::EditorUpdate()
{
	if (instance != nullptr)
	{
		instance->ProcessReceivedData();
	}
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
	started(false),
	networkObjectIDGenerator(0ULL),
	serverReceiveSpawnRequest(nullptr)
{
	InitializeWinsock();
}

NetworkManager::~NetworkManager()
{
	running.store(false);

	if (!isServer)
	{
		closesocket(connectSocket);

		if (receiveThread.joinable())
		{
			receiveThread.join();
		}
	}
	else
	{
		closesocket(listenSocket);
		for (auto& client : connectedClients)
		{
			closesocket(client.second);
		}

		for (auto& clientThread : connectedClientsReceiveThreads)
		{
			if (clientThread.second.joinable())
			{
				clientThread.second.join();
			}
		}

		if (listenThread.joinable())
		{
			listenThread.join();
		}
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
		Logger::Log(std::to_string(WSAGetLastError()), Logger::Category::Error);
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
	const char sendbuf[19] = {'1', '2', '7', '.', '0', '.', '0', '.', '1', '\0', '\0', '\0', '\0', '\0', '\0', '\0','\0', '\0', '\0' };
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
	//res = shutdown(connectSocket, SD_SEND);
	//if (res == SOCKET_ERROR) {
	//	Logger::Log("shutdown failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
	//	closesocket(connectSocket);
	//	return;
	//}

	receiveThread = std::thread(&NetworkManager::ClientReceive, this);

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
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(27015);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (res == SOCKET_ERROR)
	{
		Logger::Log("bind failed with error: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
		freeaddrinfo(result);
		closesocket(listenSocket);
		return;
	}

	freeaddrinfo(result);

	listenThread = std::thread(&NetworkManager::ListenForConnections, this);
}

void NetworkManager::ClientReceive()
{
	while (running.load())
	{
		int recvbuflen = DEFAULT_BUFFLEN;
		char recvbuf[DEFAULT_BUFFLEN] = {};

		if (!isServer)
		{
			static bool firstMessage = true;

			int iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				Logger::Log("Bytes received: " + std::to_string(iResult), Logger::Category::Success);

				if (firstMessage)
				{

					clientIP = std::string(recvbuf);

					firstMessage = false;
				}
				else if(iResult > 16)
				{
					std::lock_guard<std::mutex> guard(receivedDataMutex);

					std::string ip = std::string(recvbuf);
					std::string functionID = (recvbuf + 16);
					std::string data = ip + " " + functionID + " ";

					if (iResult > 16)
					{
						data += (recvbuf + 17 + functionID.size());
					}

					receivedData.push_back(data);
				}
			}
			else if (iResult < 0)
			{
				Logger::Log("recv failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
			}
		}
	}
}

void NetworkManager::ServerReceive(const std::string& IP)
{
	while (running.load())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		int recvbuflen = DEFAULT_BUFFLEN;
		char recvbuf[DEFAULT_BUFFLEN] = {};

		connectedClientsMutex.lock();
		const auto& clientSocket = connectedClients.find(IP);

		if (clientSocket == connectedClients.end())
		{
			return;
		}

		SOCKET socket = clientSocket->second;

		connectedClientsMutex.unlock();

		if (socket != INVALID_SOCKET)
		{
			int iResult = recv(socket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				Logger::Log("Bytes received: " + std::to_string(iResult), Logger::Category::Success);

				if (iResult >= 16)
				{
					std::lock_guard<std::mutex> guard(receivedDataMutex);

					std::string ip = std::string(recvbuf);
					std::string functionID = (recvbuf + 16);
					std::string data = ip + " " + functionID + " ";

					if (iResult > 16)
					{
						data += (recvbuf + 17 + functionID.size());
					}

					receivedData.push_back(data);
				}
			}
			else if (iResult < 0)
			{
				Logger::Log("recv failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
				closesocket(socket);
				return;
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

		
		connectedClientsMutex.lock();
		connectedClients[clientIP] = clientSocket;
		connectedClientsMutex.unlock();

		connectedClientsReceiveThreadsMutex.lock();
		connectedClientsReceiveThreads[clientIP] = std::thread(&NetworkManager::ServerReceive, this, clientIP);
		connectedClientsReceiveThreadsMutex.unlock();
		
		ServerSend(clientIP, "", "");
	}
}

void NetworkManager::ProcessReceivedData()
{
	std::lock_guard<std::mutex> guard(receivedDataMutex);

	if (!receivedData.empty())
	{
		std::string data = receivedData.front();

		if (data.size() >= 19)
		{
			std::string senderReportedIP;

			unsigned int i = 0;
			for (i; i < data.size(); i++)
			{
				if (data[i] == ' ')
				{
					break;
				}

				senderReportedIP += data[i];
			}

			std::string functionID;

			i++;
			for (i; i < data.size(); i++)
			{
				if (data[i] == ' ')
				{
					break;
				}

				functionID += data[i];
			}

			const auto& function = responseFunctions.find(functionID);

			if (function != responseFunctions.end())
			{
				if (function->second != nullptr)
				{
					(*function->second)(data);
				}
			}
		}

		receivedData.erase(receivedData.begin());
	}

}

void NetworkManager::SetupServerReceiveSpawnRequestCallback()
{
	serverReceiveSpawnRequest = new std::function<void(const std::string&)>([](const std::string& data)
		{
			std::string dataBlock = GetDataBlockFromData(data);

			std::string networkObjectClassName;
			std::string clientResponseFunctionName;

			unsigned int i = 0;
			for (i; i < dataBlock.size(); i++)
			{
				if (dataBlock[i] == ' ')
				{
					break;
				}

				networkObjectClassName += dataBlock[i];
			}

			i++;

			for (i; i < dataBlock.size(); i++)
			{
				if (dataBlock[i] == ' ')
				{
					break;
				}
				clientResponseFunctionName += dataBlock[i];
			}

			NetworkObject* newNetworkObject = nullptr;
			NetworkObject::GetConstructor(networkObjectClassName)(&newNetworkObject);

			if (newNetworkObject != nullptr)
			{
				unsigned long long newNetworkObjectID = GenerateNetworkObjectID();
				newNetworkObject->networkObjectID = newNetworkObjectID;

				newNetworkObject->OnSpawn();


				instance->spawnedNetworkObjects[newNetworkObject->networkObjectID] = newNetworkObject;

				std::string targetIP = GetIPFromData(data);

				ServerSend(targetIP, std::to_string(newNetworkObjectID), clientResponseFunctionName);
				ServerSendAll(networkObjectClassName + " " + std::to_string(newNetworkObjectID), "NetworkManagerClientReceiveSpawnFromServer", { targetIP });
			}
			else
			{
				std::string targetIP = GetIPFromData(data);

				ServerSend(targetIP, "Failure", clientResponseFunctionName);
			}
		});

	RegisterReceiveDataFunction("NetworkManagerServerSpawnRequest", serverReceiveSpawnRequest);
}

void NetworkManager::CleanupServerReceiveSpawnRequestCallback()
{
	if (serverReceiveSpawnRequest != nullptr)
	{
		delete serverReceiveSpawnRequest;
	}
}

void NetworkManager::CleanupSpawnedNetworkObjects()
{
	//for (const auto& networkObject : spawnedNetworkObjects)
	//{
	//	delete networkObject.second;
	//}
	//
	//spawnedNetworkObjects.clear();
}

void NetworkManager::SetupReceiveSpawnFromServer()
{
	receiveSpawnFromServer = new std::function<void(const std::string&)>([](const std::string& data)
		{
			std::string dataBlock = GetDataBlockFromData(data);

			std::string networkObjectClassName;
			std::string networkObjectID;

			unsigned int i = 0;
			for (i; i < dataBlock.size(); i++)
			{
				if (i == ' ')
				{
					break;
				}

				networkObjectClassName += dataBlock[i];
			}

			i++;

			for (i; i < dataBlock.size(); i++)
			{
				networkObjectID += dataBlock[i];
			}

			NetworkObject* newNetworkObject = nullptr;
			NetworkObject::GetConstructor(networkObjectClassName)(&newNetworkObject);

			if (newNetworkObject != nullptr)
			{
				newNetworkObject->networkObjectID = std::stoull(networkObjectID);
				newNetworkObject->OnSpawn();
				instance->spawnedNetworkObjects[newNetworkObject->networkObjectID] = newNetworkObject;
			}
		});

	RegisterReceiveDataFunction("NetworkManagerClientReceiveSpawnFromServer", receiveSpawnFromServer);
}

void NetworkManager::CleanupReceiveSpawnFromServer()
{
	if (receiveSpawnFromServer != nullptr)
	{
		DeregisterReceiveDataFunction("NetworkManagerClientReceiveSpawnFromServer");
		delete receiveSpawnFromServer;
	}
}

void NetworkManager::ServerSendAll(const std::string& data, const std::string& receiveFunction, const std::unordered_set<std::string>& excludedIPs)
{
	if (instance != nullptr)
	{
		if (IsServer())
		{
			instance->connectedClientsMutex.lock();
			for (auto& clientSocket : instance->connectedClients)
			{
				if (excludedIPs.contains(clientSocket.first))
				{
					continue;
				}

				char* sendbuf = new char[16 + receiveFunction.size() + data.size() + 1];
				ZeroMemory(sendbuf, 16 + receiveFunction.size() + data.size() + 1);

				unsigned int i = 0;
				for (i; i < clientSocket.first.size(); i++)
				{
					*(sendbuf + i) = clientSocket.first[i];
				}
				
				i = 16;
				for (i; i < 16 + receiveFunction.size(); i++)
				{
					*(sendbuf + i) = receiveFunction[i - 16];
				}

				*(sendbuf + i) = '\0';

				i = 17 + receiveFunction.size();
				for (i; i < 17 + receiveFunction.size() + data.size(); i++)
				{
					*(sendbuf + i) = data[i - (17 + receiveFunction.size())];
				}

				int iSendResult = send(clientSocket.second, sendbuf, 16 + receiveFunction.size() + data.size() + 1, 0);
				if (iSendResult == SOCKET_ERROR) {
					Logger::Log("send failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
					closesocket(clientSocket.second);
					return;
				}

				delete[] sendbuf;
				Logger::Log("Bytes sent: " + std::to_string(iSendResult) + " to: " + clientSocket.first, Logger::Category::Success);
			}
			instance->connectedClientsMutex.unlock();
		}
	}
}

void NetworkManager::ServerSend(const std::string& ip, const std::string& data, const std::string& receiveFunction)
{
	if (instance != nullptr)
	{
		if (IsServer())
		{
			instance->connectedClientsMutex.lock();
			const auto& clientSocket = instance->connectedClients.find(ip);

			if (clientSocket != instance->connectedClients.end())
			{
				char* sendbuf = new char[16 + receiveFunction.size() + data.size() + 1];

				ZeroMemory(sendbuf, 16 + receiveFunction.size() + data.size() + 1);

				unsigned int i = 0;
				for (i; i < ip.size(); i++)
				{
					*(sendbuf + i) = ip[i];
				}

				i = 16;
				for (i; i < 16 + receiveFunction.size(); i++)
				{
					*(sendbuf + i) = receiveFunction[i - 16];
				}

				*(sendbuf + i) = '\0';

				i = 17 + receiveFunction.size();
				for (i; i < 17 + receiveFunction.size() + data.size(); i++)
				{
					*(sendbuf + i) = data[i - (17 + receiveFunction.size())];
				}

				int iSendResult = send(clientSocket->second, sendbuf, 16 + receiveFunction.size() + data.size() + 1, 0);
				if (iSendResult == SOCKET_ERROR) {
					Logger::Log("send failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
					closesocket(clientSocket->second);
					return;
				}

				delete[] sendbuf;

				Logger::Log("Bytes sent: " + std::to_string(iSendResult) + " to: " + clientSocket->first, Logger::Category::Success);
			}

			

			instance->connectedClientsMutex.unlock();
		}
	}
}

void NetworkManager::ClientSend(const std::string& data, const std::string& receiveFunction)
{
	if (instance != nullptr)
	{
		if (!IsServer())
		{
			char* sendbuf = new char[16 + receiveFunction.size() + data.size() + 1];
			ZeroMemory(sendbuf, 16 + receiveFunction.size() + data.size() + 1);
			
			unsigned int i = 0;
			for (i; i < instance->clientIP.size(); i++)
			{
				*(sendbuf + i) = instance->clientIP[i];
			}

			i = 16;
			for (i; i < 16 + receiveFunction.size(); i++)
			{
				*(sendbuf + i) = receiveFunction[i - 16];
			}

			*(sendbuf + i) = '\0';

			i = 17 + receiveFunction.size();
			for (i; i < 17 + receiveFunction.size() + data.size(); i++)
			{
				*(sendbuf + i) = data[i - (17 + receiveFunction.size())];
			}

			int res = send(instance->connectSocket, sendbuf, 16 + receiveFunction.size() + data.size() + 1, 0);
			if (res == SOCKET_ERROR) {
				Logger::Log("send failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
				closesocket(instance->connectSocket);
				return;
			}

			delete[] sendbuf;
		}
	}
}

void NetworkManager::RegisterReceiveDataFunction(const std::string& key, std::function<void(const std::string&)>* function)
{
	if (instance != nullptr)
	{
		instance->responseFunctions[key] = function;
	}
}

void NetworkManager::DeregisterReceiveDataFunction(const std::string& key)
{
	if (instance != nullptr)
	{
		instance->responseFunctions.erase(instance->responseFunctions.find(key));
	}
}

std::string NetworkManager::GetIPFromData(const std::string& data)
{
	std::string IP;

	unsigned int i = 0;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		IP += data[i];
	}

	return IP;
}

std::string NetworkManager::GetFunctionFromData(const std::string& data)
{
	std::string IP;

	unsigned int i = 0;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		IP += data[i];
	}
	i++;

	std::string functionID;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		functionID += data[i];
	}

	return functionID;
}

std::string NetworkManager::GetDataBlockFromData(const std::string data)
{
	std::string IP;

	unsigned int i = 0;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		IP += data[i];
	}

	i++;

	std::string functionID;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		functionID += data[i];
	}

	i++;

	std::string ret = std::string(data.begin() + i, data.end());

	return ret;
}

std::string NetworkManager::ConvertVec3ToData(const glm::vec3& vec3)
{
	return std::to_string(vec3.x) + " " + std::to_string(vec3.y) + " " + std::to_string(vec3.z);
}

glm::vec3 NetworkManager::ConvertDataToVec3(const std::string& data)
{
	std::string x = "";
	std::string y = "";
	std::string z = "";

	unsigned int i = 0;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		x += data[i];
	}

	i++;

	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		y += data[i];
	}

	i++;

	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			break;
		}

		z += data[i];
	}

	return glm::vec3(std::stof(x), std::stof(y), std::stof(z));
}

std::string NetworkManager::GetIP()
{
	if (instance != nullptr)
	{
		if (IsServer())
		{
			return SERVER_IP;
		}
		else
		{
			return instance->clientIP;
		}
	}

	return "";
}

unsigned long long NetworkManager::GenerateNetworkObjectID()
{
	if (instance != nullptr)
	{

		if (IsServer())
		{
			return instance->networkObjectIDGenerator++;
		}
		else
		{
			Logger::Log("Calling NetworkManager::GenerateNetworkObjectID on a Client. This should only be called on the server.");
		}

	}

	Logger::Log("Calling NetworkManager::GenerateNetworkObjectID before NetworkManager::Initialize", Logger::Category::Error);
	return ULLONG_MAX;
}

void NetworkManager::Spawn(const std::string& networkObjectClassName, std::function<void(NetworkObject*)>* callback)
{
	if (instance == nullptr)
	{
		return;
	}


	if (IsServer())
	{
		
	}
	else
	{
		
		static unsigned long long clientSpawnRequestID = 0ULL;

		std::function<void(const std::string&)>* clientSpawnRequest = new std::function<void(const std::string&)>([networkObjectClassName, callback](const std::string& data)
			{
				std::string dataBlock = GetDataBlockFromData(data);

				if (dataBlock != "Failure")
				{
					NetworkObject* newNetworkObject = nullptr;
					NetworkObject::GetConstructor(networkObjectClassName)(&newNetworkObject);

					if (newNetworkObject != nullptr)
					{
						newNetworkObject->OnSpawn();
						newNetworkObject->networkObjectID = std::stod(dataBlock);
						instance->spawnedNetworkObjects[newNetworkObject->networkObjectID] = newNetworkObject;
					}

					(*callback)(newNetworkObject);
				}
			});

		instance->clientSpawnRequestCallbacks[clientSpawnRequestID++] = clientSpawnRequest;
		
		std::string clientSpawnRequestName = "ClientSpawnRequest:" + std::to_string(clientSpawnRequestID);

		RegisterReceiveDataFunction(clientSpawnRequestName, clientSpawnRequest);


		ClientSend(networkObjectClassName + " " + clientSpawnRequestName, "NetworkManagerServerSpawnRequest");
	}
}
