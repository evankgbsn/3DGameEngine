#include "NetworkManager.h"

#include "../Utils/SingletonHelpers.h"
#include "NetworkObject.h"
#include "../GameObject/GameObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"

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
	instance->SetupServerReceiveDespawnRequestCallback();
	instance->SetupReceiveDespawnFromServer();
	instance->SetupSpawnConfirmationCallbacks();
	instance->SetupSyncCallbacks();
}

void NetworkManager::Terminate()
{
	instance->CleanupReceiveDespawnFromServer();
	instance->CleanupServerReceiveDespawnRequestCallback();
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
		instance->ProcessMainThreadUpdates();
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
		shutdown(connectSocket, SD_SEND);

		if (receiveThread.joinable())
		{
			receiveThread.join();
		}

		closesocket(connectSocket);
	}
	else
	{
		closesocket(listenSocket);
		for (auto& client : connectedClients)
		{
			shutdown(client.second, SD_SEND);
		}

		for (auto& clientThread : connectedClientsReceiveThreads)
		{
			if (clientThread.second.joinable())
			{
				clientThread.second.join();
			}
		}

		for (auto& client : connectedClients)
		{
			closesocket(client.second);
		}

		if (listenThread.joinable())
		{
			listenThread.join();
		}

		if (cleanDisconnectedClientThread.joinable())
		{
			cleanDisconnectedClientThread.join();
		}

		CleanupClientSpawnRequests();
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
	char sendbuf[23] = {' ', ' ',' ', ' ','1', '2', '7', '.', '0', '.', '0', '.', '1', '\0', '\0', '\0', '\0', '\0', '\0', '\0','\0', '\0', '\0'};
	char recvbuf[DEFAULT_BUFFLEN];

	*reinterpret_cast<unsigned int*>(sendbuf) = 23;

	// Send an initial buffer
	res = send(connectSocket, sendbuf, sizeof(sendbuf), 0);
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
	cleanDisconnectedClientThread = std::thread(&NetworkManager::CleanDisconnectedClientThreads, this);
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
				//Logger::Log("Bytes received: " + std::to_string(iResult), Logger::Category::Success);

				unsigned int packetSize = *reinterpret_cast<unsigned int*>(recvbuf);

				if (firstMessage)
				{
					clientIP = std::string(recvbuf + 4);

					firstMessage = false;
				}
				else if(iResult > 20)
				{
					std::lock_guard<std::mutex> guard(receivedDataMutex);

					unsigned int x = 0;
					while (x < iResult)
					{
						
						// There are not enough bytes left to get the packet size.
						if (512 - x < 4)
						{
							unsigned int missingSizeBytes = 4 - (512 - x);

							char* partialPacketSizeBuf = new char[4]('\0');

							unsigned int i = 0;
							for (i; i < 512 - x; i++)
							{
								partialPacketSizeBuf[i] = *(recvbuf + x + i);
							}

							// Get the rest of the packet size.
							int iResult = recv(connectSocket, partialPacketSizeBuf + 512 - x, missingSizeBytes, 0);

							if (iResult == missingSizeBytes)
							{
								missingSizeBytes = 0;
							}

						}

						packetSize = *reinterpret_cast<unsigned int*>(recvbuf + x);

						if (packetSize > 20)
						{
							if (iResult < packetSize)
							{
								packetSize = 0;
							}

							std::string ip = std::string(recvbuf + 4 + x);
							std::string functionID = (recvbuf + 20 + x);
							std::string data = ip + " " + functionID + " ";

							unsigned int i = 21 + functionID.size() + x;
							while (i < packetSize + x && i < iResult)
							{
								data += recvbuf[i];
								i++;
							}

							if (i <= iResult + 1)
							{
								// Full packet
								receivedData.push_back(data);
							}
							else
							{
								partialPacket:
								
								unsigned int startOfPacket = i - x;

								char* packetRecvBuf = new char[packetSize]('\0');
								ZeroMemory(packetRecvBuf, packetSize);

								unsigned int j = x;
								while (j < iResult)
								{
									packetRecvBuf[j - x] = recvbuf[j];
									j++;
								}

								// Partial packet
								unsigned int remaining = packetSize - (iResult - x);
								iResult = recv(connectSocket, packetRecvBuf + iResult - x, remaining , 0);
								if (iResult > 0)
								{
									if (iResult < remaining)
									{
										packetSize = 0;
									}

									std::string ip = std::string(packetRecvBuf + 4);
									std::string functionID = (packetRecvBuf + 20);
									std::string data = ip + " " + functionID + " ";

									unsigned int i = 21 + functionID.size();
									while (i < packetSize)
									{
										data += packetRecvBuf[i];
										i++;
									}

									receivedData.push_back(data);
									break;
								}
								else if (iResult == 0)
								{
									Logger::Log("Disconnected from server", Logger::Category::Warning);
									return;
								}
								else if (iResult < 0)
								{
									Logger::Log("recv failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
									return;
								}
							}
							x = i;
						}
					}
				}
			}
			else if (iResult == 0)
			{
				Logger::Log("Disconnected from server", Logger::Category::Warning);
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

		ZeroMemory(recvbuf, DEFAULT_BUFFLEN);

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
				//Logger::Log("Bytes received: " + std::to_string(iResult), Logger::Category::Success);

				std::lock_guard<std::mutex> guard(receivedDataMutex);

				unsigned int packetSize = *reinterpret_cast<unsigned int*>(recvbuf);

				unsigned int x = 0;
				unsigned int iteration = 0;
				while (x < iResult)
				{
				newPacket:
					packetSize = *reinterpret_cast<unsigned int*>(recvbuf + x);

					if (packetSize > 20)
					{
						std::string ip = std::string(recvbuf + 4 + x);
						std::string functionID = (recvbuf + 20 + x);
						std::string data = ip + " " + functionID + " ";

						unsigned int i = 21 + functionID.size() + x;
						while (i < packetSize + x && i < iResult)
						{
							data += recvbuf[i];
							i++;
						}

						if (i <= iResult + 1)
						{
							// Full packet
							receivedData.push_back(data);
						}
						else
						{
							// Partial packet
							ZeroMemory(recvbuf, recvbuflen);
							iResult = recv(socket, recvbuf, recvbuflen, 0);
							if (iResult > 0)
							{
								i = 0;
								while (i < packetSize + x && i < iResult)
								{
									data += recvbuf[i];
									i++;
								}

								if (i < iResult)
								{
									x = i;
									goto newPacket;
								}
							}
							else if (iResult == 0)
							{
								Logger::Log("Client Disconnected: " + IP, Logger::Category::Warning);
								closesocket(socket);
								connectedClientsMutex.lock();
								connectedClients.erase(clientSocket);
								connectedClientsMutex.unlock();

								OnClientDisconnect(IP);
								return;
							}
							else if (iResult < 0)
							{
								if (WSAGetLastError() == WSAECONNRESET)
								{
									Logger::Log("Client Hard Disconnected: " + IP, Logger::Category::Error);
									closesocket(socket);
									connectedClientsMutex.lock();
									connectedClients.erase(clientSocket);
									connectedClientsMutex.unlock();

									OnClientDisconnect(IP);
									return;
								}
								else
								{
									Logger::Log("recv failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
									closesocket(socket);
									return;
								}
							}
						}
						x = i;
						iteration++;
					}
				}
			}
			else if (iResult == 0)
			{
				Logger::Log("Client Disconnected: " + IP, Logger::Category::Warning);
				closesocket(socket);
				connectedClientsMutex.lock();
				connectedClients.erase(clientSocket);
				connectedClientsMutex.unlock();

				OnClientDisconnect(IP);
				return;
			}
			else if (iResult < 0)
			{
				if (WSAGetLastError() == WSAECONNRESET)
				{
					Logger::Log("Client Hard Disconnected: " + IP, Logger::Category::Error);
					closesocket(socket);
					connectedClientsMutex.lock();
					connectedClients.erase(clientSocket);
					connectedClientsMutex.unlock();

					OnClientDisconnect(IP);
					return;
				}
				else
				{
					Logger::Log("recv failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
					closesocket(socket);
					return;
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

		
		connectedClientsMutex.lock();
		connectedClients[clientIP] = clientSocket;
		connectedClientsMutex.unlock();

		connectedClientsReceiveThreadsMutex.lock();
		connectedClientsReceiveThreads[clientIP] = std::thread(&NetworkManager::ServerReceive, this, clientIP);
		connectedClientsReceiveThreadsMutex.unlock();

		Logger::Log("Client Connected: " + std::string(clientIP), Logger::Category::Info);
		
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

			if (functionID.empty())
			{
				Logger::Log(std::string(functionID));
			}
			Logger::Log(std::string(functionID));

			const auto& function = responseFunctions.find(functionID);

			if (function != responseFunctions.end())
			{
				if (function->second != nullptr)
				{
					(*function->second)(data);
				}
			}
			else
			{
				Logger::Log("Cannot Process Packet. Either incorrect function ID or corrupt packet: " + data, Logger::Category::Warning);
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

				instance->spawnedNetworkObjects[newNetworkObject->networkObjectID] = newNetworkObject;

				std::string targetIP = GetIPFromData(data);
				newNetworkObject->spawnerIP = targetIP;

				newNetworkObject->OnSpawn();

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

void NetworkManager::SetupServerReceiveDespawnRequestCallback()
{
	serverReceiveDespawnRequest = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			std::string dataBlock = GetDataBlockFromData(data);

			std::string networkObjectID;
			std::string clientResponseFunctionName;

			unsigned int i = 0;
			for (i; i < dataBlock.size(); i++)
			{
				if (dataBlock[i] == ' ')
				{
					break;
				}

				networkObjectID += dataBlock[i];
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

			const auto& networkObject = spawnedNetworkObjects.find(std::stoull(networkObjectID));

			if (networkObject != spawnedNetworkObjects.end())
			{
				networkObject->second->OnDespawn();

				GameObject* networkGameObject = dynamic_cast<GameObject*>(networkObject->second);

				if (networkGameObject != nullptr)
				{
					Scene* owningScene = networkGameObject->GetOwningScene();
					if (owningScene != nullptr)
					{
						owningScene->DeregisterGameObject(networkGameObject->GetName());
					}
				}

				delete networkObject->second;
				spawnedNetworkObjects.erase(networkObject);

				std::string targetIP = GetIPFromData(data);

				ServerSendAll(networkObjectID, "NetworkManagerClientReceiveDespawnFromServer");
			}
			else
			{
				Logger::Log("Received request to despawn a network object that was never spawned with network object ID: " + std::stoull(networkObjectID), Logger::Category::Warning);
			}
		});

	RegisterReceiveDataFunction("NetworkManagerServerDespawnRequest", serverReceiveDespawnRequest);
}

void NetworkManager::CleanupServerReceiveDespawnRequestCallback()
{
	if (serverReceiveDespawnRequest != nullptr)
	{
		DeregisterReceiveDataFunction("NetworkManagerServerDespawnRequest");
		delete serverReceiveDespawnRequest;
	}
}

void NetworkManager::CleanupSpawnedNetworkObjects()
{
	for (const auto& networkObject : spawnedNetworkObjects)
	{
		delete networkObject.second;
	}
	
	spawnedNetworkObjects.clear();
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
				if (dataBlock[i] == ' ')
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
				instance->spawnedNetworkObjects[newNetworkObject->networkObjectID] = newNetworkObject;
				newNetworkObject->OnSpawn();

				// Send confirmation to server.
				ClientSend(std::to_string(newNetworkObject->GetNetworkObjectID()), "ServerSpawnConfirmation");
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

void NetworkManager::SetupReceiveDespawnFromServer()
{
	receiveDespawnFromServer = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			std::string dataBlock = GetDataBlockFromData(data);

			std::string networkObjectID;

			unsigned int i = 0;
			for (i; i < dataBlock.size(); i++)
			{
				if (dataBlock[i] == ' ')
				{
					break;
				}

				networkObjectID += dataBlock[i];
			}

			const auto& networkObject = spawnedNetworkObjects.find(std::stoull(networkObjectID));

			if (networkObject != spawnedNetworkObjects.end())
			{
				GameObject* networkGameObject = dynamic_cast<GameObject*>(networkObject->second);

				if (networkGameObject != nullptr)
				{
					Scene* owningScene = networkGameObject->GetOwningScene();
					if (owningScene != nullptr)
					{
						owningScene->DeregisterGameObject(networkGameObject->GetName());
					}
				}

				delete networkObject->second;
				spawnedNetworkObjects.erase(networkObject);
			}

		});

	RegisterReceiveDataFunction("NetworkManagerClientReceiveDespawnFromServer", receiveDespawnFromServer);
}

void NetworkManager::CleanupReceiveDespawnFromServer()
{
	if (receiveDespawnFromServer != nullptr)
	{
		DeregisterReceiveDataFunction("NetworkManagerClientReceiveDespawnFromServer");
		delete receiveDespawnFromServer;
	}
}

void NetworkManager::OnClientDisconnect(const std::string& IP)
{
	std::lock_guard<std::mutex> guard(onClientDisconnectCallbacksMutex);

	for (const auto& callback : onClientDisconnectCallbacks)
	{
		std::lock_guard<std::mutex> guard(mainThreadUpdatesMutex);

		mainThreadUpdates.push_back([callback, IP]()
			{
				(*callback.second)(IP);
			});
	}
}

void NetworkManager::CleanDisconnectedClientThreads()
{
	while (running.load())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		std::unordered_set<std::string> disconnectedClients;

		connectedClientsReceiveThreadsMutex.lock();

		for (auto& clientThread : connectedClientsReceiveThreads)
		{
			if (connectedClients.find(clientThread.first) == connectedClients.end())
			{
				disconnectedClients.insert(clientThread.first);

				if (clientThread.second.joinable())
				{
					clientThread.second.join();
				}
			}
		}

		for (const auto& client : disconnectedClients)
		{
			connectedClientsReceiveThreads.erase(connectedClientsReceiveThreads.find(client));
		}


		connectedClientsReceiveThreadsMutex.unlock();
	}
}

void NetworkManager::ProcessMainThreadUpdates()
{
	for (const auto& update : mainThreadUpdates)
	{
		update();
	}

	mainThreadUpdates.clear();
}

void NetworkManager::CleanupClientSpawnRequests()
{
	for (const auto& function : clientSpawnRequestCallbacks)
	{
		std::string clientSpawnRequestName = "ClientSpawnRequest:" + std::to_string(function.first);
		DeregisterReceiveDataFunction(clientSpawnRequestName);
		delete function.second;
	}
}

void NetworkManager::SetupSpawnConfirmationCallbacks()
{
	static std::function<void(const std::string&)> serverSpawnConfirmation = [this](const std::string& data)
		{
			std::string IP = GetIPFromData(data);
			std::string dataBlock = GetDataBlockFromData(data);

			unsigned long long id = std::stoull(dataBlock);

			const auto& object = spawnedNetworkObjects.find(id);

			if (object != spawnedNetworkObjects.end())
			{
				object->second->OnServerSpawnConfirmation(IP);
				ServerSend(IP, dataBlock, "ClientSpawnConfirmation");
			}
		};

	RegisterReceiveDataFunction("ServerSpawnConfirmation", &serverSpawnConfirmation);

	static std::function<void(const std::string&)> clientSpawnConfirmation = [this](const std::string& data)
		{
			std::string IP = GetIPFromData(data);
			std::string dataBlock = GetDataBlockFromData(data);

			unsigned long long id = std::stoull(dataBlock);

			const auto& object = spawnedNetworkObjects.find(id);

			if (object != spawnedNetworkObjects.end())
			{
				object->second->OnClientSpawnConfirmation();
			}
		};

	RegisterReceiveDataFunction("ClientSpawnConfirmation", &clientSpawnConfirmation);

}

void NetworkManager::SetupSyncCallbacks()
{
	static std::function<void(const std::string&)> serverReceiveSyncRequest = [this](const std::string& data)
		{
			std::string IP = GetIPFromData(data);

			std::vector<std::string> sceneNames;

			std::string dataBlock = GetDataBlockFromData(data);

			unsigned int index = 0;
			while (index < dataBlock.size())
			{
				std::string sceneName;
				while (dataBlock[index] != ' ')
				{
					sceneName.append({ dataBlock[index] });

					index++;
					if (index >= dataBlock.size())
					{
						break;
					}
				}

				sceneNames.push_back(sceneName);
				index++;
			}

			std::vector<NetworkObject*> networkObjectsToSend;

			for (const std::string& sceneName : sceneNames)
			{
				Scene* scene = SceneManager::GetLoadedScene(sceneName);

				if (scene != nullptr)
				{
					const auto& gameObjects = scene->GetGameObjects();

					for (const auto& gameObject : gameObjects)
					{
						NetworkObject* netObject = dynamic_cast<NetworkObject*>(gameObject.second);

						if (netObject != nullptr)
						{
							networkObjectsToSend.push_back(netObject);
						}
					}
				}
			}

			std::string netObjectDataToSend;

			for (unsigned int i = 0; i < networkObjectsToSend.size(); i++)
			{
				if (i != 0)
				{
					netObjectDataToSend += " ";
				}

				NetworkObject* netObject = networkObjectsToSend[i];

				GameObject* gameObject = dynamic_cast<GameObject*>(netObject);

				netObjectDataToSend.append(netObject->nameOfType + " " + std::to_string(netObject->GetNetworkObjectID()) + " " + gameObject->GetOwningScene()->GetName());
			}

			ServerSend(IP, netObjectDataToSend, "ClientReceiveSync");
		};

	RegisterReceiveDataFunction("ServerReceiveSyncRequest", &serverReceiveSyncRequest);

	static std::function<void(const std::string&)> clientReceiveSync = [this](const std::string& data)
		{
			std::string dataBlock = GetDataBlockFromData(data);

			unsigned int i = 0;
			while (i < dataBlock.size())
			{
				std::string objectTypeName;

				while (dataBlock[i] != ' ')
				{
					objectTypeName += dataBlock[i];
					i++;
				}

				i++;

				std::string networkObjectID;

				while (dataBlock[i] != ' ')
				{
					networkObjectID += dataBlock[i];
					i++;
				}

				i++;

				std::string owningScene;

				while (dataBlock[i] != ' ')
				{
					owningScene += dataBlock[i];
					i++;
					if (i >= dataBlock.size())
					{
						break;
					}
				}

				i++;

				Scene* scene = SceneManager::GetLoadedScene(owningScene);

				if (scene != nullptr && spawnedNetworkObjects.find(std::stoull(networkObjectID)) == spawnedNetworkObjects.end())
				{
					NetworkObject* newNetworkObject = nullptr;
					NetworkObject::GetConstructor(objectTypeName)(&newNetworkObject);

					if (newNetworkObject != nullptr)
					{
						newNetworkObject->networkObjectID = std::stoull(networkObjectID);
						newNetworkObject->OnSpawn();
						instance->spawnedNetworkObjects[newNetworkObject->networkObjectID] = newNetworkObject;

						// Send confirmation to server.
						ClientSend(std::to_string(newNetworkObject->GetNetworkObjectID()), "ServerSpawnConfirmation");
					}
				}
			}
		};

	RegisterReceiveDataFunction("ClientReceiveSync", &clientReceiveSync);
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

				unsigned int packetSize = 16 + receiveFunction.size() + data.size() + 1 + 4;

				char* sendbuf = new char[packetSize]('\0');
				ZeroMemory(sendbuf, packetSize);

				*reinterpret_cast<unsigned int*>(sendbuf) = packetSize;

				unsigned int i = 4;
				for (i; i < instance->clientIP.size(); i++)
				{
					*(sendbuf + i) = instance->clientIP[i];
				}

				i = 20;
				for (i; i < 20 + receiveFunction.size(); i++)
				{
					*(sendbuf + i) = receiveFunction[i - 20];
				}

				*(sendbuf + i) = '\0';

				i = 21 + receiveFunction.size();
				for (i; i < 21 + receiveFunction.size() + data.size(); i++)
				{
					*(sendbuf + i) = data[i - (21 + receiveFunction.size())];
				}

				int iSendResult = send(clientSocket.second, sendbuf, packetSize, 0);
				if (iSendResult == SOCKET_ERROR) {
					Logger::Log("send failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
					closesocket(clientSocket.second);
					return;
				}

				delete[] sendbuf;
				//Logger::Log("Bytes sent: " + std::to_string(iSendResult) + " to: " + clientSocket.first, Logger::Category::Success);
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
				unsigned int packetSize = 16 + receiveFunction.size() + data.size() + 1 + 4;

				char* sendbuf = new char[packetSize]('\0');
				ZeroMemory(sendbuf, packetSize);

				*reinterpret_cast<unsigned int*>(sendbuf) = packetSize;

				unsigned int i = 4;
				for (i; i < ip.size() + 4; i++)
				{
					*(sendbuf + i) = ip[i - 4];
				}

				i = 20;
				for (i; i < 20 + receiveFunction.size(); i++)
				{
					*(sendbuf + i) = receiveFunction[i - 20];
				}

				*(sendbuf + i) = '\0';

				i = 21 + receiveFunction.size();
				for (i; i < 21 + receiveFunction.size() + data.size(); i++)
				{
					*(sendbuf + i) = data[i - (21 + receiveFunction.size())];
				}

				int iSendResult = send(clientSocket->second, sendbuf, packetSize, 0);
				if (iSendResult == SOCKET_ERROR) {
					Logger::Log("send failed: " + std::to_string(WSAGetLastError()), Logger::Category::Error);
					closesocket(clientSocket->second);
					return;
				}

				delete[] sendbuf;

				//Logger::Log("Bytes sent: " + std::to_string(iSendResult) + " to: " + clientSocket->first, Logger::Category::Success);
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
			unsigned int packetSize = 16 + receiveFunction.size() + data.size() + 1 + 4;

			char* sendbuf = new char[packetSize]('\0');
			ZeroMemory(sendbuf, packetSize);
			
			*reinterpret_cast<unsigned int*>(sendbuf) = packetSize;

			unsigned int i = 4;
			for (i; i < instance->clientIP.size() + 4; i++)
			{
				*(sendbuf + i) = instance->clientIP[i - 4];
			}

			i = 20;
			for (i; i < 20 + receiveFunction.size(); i++)
			{
				*(sendbuf + i) = receiveFunction[i - 20];
			}

			*(sendbuf + i) = '\0';

			i = 21 + receiveFunction.size();
			for (i; i < 21 + receiveFunction.size() + data.size(); i++)
			{
				*(sendbuf + i) = data[i - (21 + receiveFunction.size())];
			}

			Logger::Log(std::string(receiveFunction));

			int res = send(instance->connectSocket, sendbuf, packetSize, 0);
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

void NetworkManager::RegisterOnClientDisconnectFunction(const std::string& key, std::function<void(const std::string&)>* callback)
{
	if (instance != nullptr)
	{
		std::lock_guard<std::mutex> guard(instance->onClientDisconnectCallbacksMutex);
		instance->onClientDisconnectCallbacks[key] = callback;
	}
}

void NetworkManager::DeregisterOnClientDisconnectFunction(const std::string& key)
{
	if (instance != nullptr)
	{
		std::lock_guard<std::mutex> guard(instance->onClientDisconnectCallbacksMutex);
		instance->onClientDisconnectCallbacks.erase(instance->onClientDisconnectCallbacks.find(key));
	}
}

void NetworkManager::RegisterOnDisconnectFunction(const std::string& key, std::function<void()>* callback)
{
	if (instance != nullptr)
	{
		instance->onDisconnectCallbacks[key] = callback;
	}
}

void NetworkManager::DeregisterOnDisconnectFunction(const std::string& key)
{
	if (instance != nullptr)
	{
		instance->onDisconnectCallbacks.erase(instance->onDisconnectCallbacks.find(key));
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
						newNetworkObject->spawnedFromLocalSpawnRequest = true;
						newNetworkObject->networkObjectID = std::stod(dataBlock);
						newNetworkObject->OnSpawn();
						instance->spawnedNetworkObjects[newNetworkObject->networkObjectID] = newNetworkObject;

						// Send confirmation to server.
						ClientSend(std::to_string(newNetworkObject->GetNetworkObjectID()), "ServerSpawnConfirmation");
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

void NetworkManager::Despawn(unsigned long long networkObjectID)
{
	if (instance == nullptr)
	{
		return;
	}

	if (IsServer())
	{
		const auto& networkObject = instance->spawnedNetworkObjects.find(networkObjectID);

		if (networkObject != instance->spawnedNetworkObjects.end())
		{
			GameObject* networkGameObject = dynamic_cast<GameObject*>(networkObject->second);

			if (networkGameObject != nullptr)
			{
				Scene* owningScene = networkGameObject->GetOwningScene();
				if (owningScene != nullptr)
				{
					owningScene->DeregisterGameObject(networkGameObject->GetName());
				}
			}

			networkObject->second->OnDespawn();

			delete networkObject->second;
			instance->spawnedNetworkObjects.erase(networkObject);
		}

		ServerSendAll(std::to_string(networkObjectID), "NetworkManagerClientReceiveDespawnFromServer");
	}
	else
	{
		ClientSend(std::to_string(networkObjectID), "NetworkManagerServerDespawnRequest");
	}
}

void NetworkManager::SyncClientWithServer()
{
	const std::vector<std::string> loadedScenes = SceneManager::GetLoadedSceneNames();

	std::string dataToSend;

	unsigned int i = 0;
	for (i; i < loadedScenes.size(); i++)
	{
		if (i != 0)
		{
			dataToSend.append(" ");
		}

		dataToSend.append(loadedScenes[i]);
	}

	Logger::Log(std::string("Sending sync request"));
	ClientSend(dataToSend, "ServerReceiveSyncRequest");
}
