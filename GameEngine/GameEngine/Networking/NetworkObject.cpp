#include "NetworkObject.h"

#include "NetworkManager.h"

std::unordered_map<std::string, std::function<void(NetworkObject**)>> NetworkObject::newFunctions = std::unordered_map<std::string, std::function<void(NetworkObject**)>>();

const std::string& NetworkObject::FormatPacket(const std::string& type, unsigned long long packetNumber, const std::string& data)
{
	return type + " " + std::to_string(packetNumber) + " " + data;
}

void NetworkObject::OnServerSpawnConfirmation(const std::string& IP)
{
	serverConfirmedIDs.insert(IP);
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

void NetworkObject::ServerSend(const std::string& ID, const std::string& data, bool reliable)
{
	if (serverConfirmedIDs.contains(ID))
	{
		if (reliable)
		{
			NetworkManager::ServerSend(ID, data, "NetworkObject:" + std::to_string(networkObjectID));
		}
		else
		{
			NetworkManager::ServerSendUDP(ID, data, "NetworkObjectUDP:" + std::to_string(networkObjectID));
		}
	}
}

void NetworkObject::ServerSendAll(const std::string& data, const std::unordered_set<std::string>& excludedIPs, bool reliable)
{
	for (const std::string& ip : serverConfirmedIDs)
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

std::string NetworkObject::GetSpawnerID() const
{
	return spawnerID;
}

void NetworkObject::AddServerDataReceivedCallback(const std::string& type, std::function<void(const std::string&)>* callback)
{
	serverDataReceivedFunctions[type] = callback;
	serverLastPacketIDs[type] = 0U;
}

void NetworkObject::RemoveServerDataReceivedCallback(const std::string& type)
{
	if (serverDataReceivedFunctions.find(type) != serverDataReceivedFunctions.end())
	{
		serverDataReceivedFunctions.erase(serverDataReceivedFunctions.find(type));
		serverLastPacketIDs.erase(serverLastPacketIDs.find(type));
	}
}

void NetworkObject::AddClientDataReceivedCallback(const std::string& type, std::function<void(const std::string&)>* callback)
{
	clientDataReceivedFunctions[type] = callback;
	clientLastPacketIDs[type] = 0U;
}

void NetworkObject::RemoveClientDataReceivedCallback(const std::string& type)
{
	if (clientDataReceivedFunctions.find(type) != clientDataReceivedFunctions.end())
	{
		clientDataReceivedFunctions.erase(clientDataReceivedFunctions.find(type));
		clientLastPacketIDs.erase(clientLastPacketIDs.find(type));
	}
}

bool NetworkObject::IsServer() const
{
	return NetworkManager::IsServer();
}

bool NetworkObject::IsClient() const
{
	return !IsServer();
}

bool NetworkObject::IsLocalClient() const
{
	return SpawnedFromLocalSpawnRequest() && IsClient();
}

void NetworkObject::OnDataReceived(const std::string& data)
{
	std::string packetIDstr;
	std::string type;
	std::string updateData;

	ProcessReceivedData(data, type, packetIDstr, updateData);

	unsigned long long packetID = std::stoull(packetIDstr);

	if (NetworkManager::IsServer())
	{
		if (serverDataReceivedFunctions.find(type) != serverDataReceivedFunctions.end())
		{
			if (packetID >= serverLastPacketIDs[type])
			{
				(*serverDataReceivedFunctions[type])(updateData);
			}
			else
			{
				Logger::Log(std::string("Lost or out of order packet: ") + type, Logger::Category::Warning);
			}

			serverLastPacketIDs[type] = packetID;
		}
	}
	else
	{
		if (clientDataReceivedFunctions.find(type) != clientDataReceivedFunctions.end())
		{
			if (packetID >= clientLastPacketIDs[type])
			{
				(*clientDataReceivedFunctions[type])(updateData);
			}
			else
			{
				Logger::Log(std::string("Lost or out of order packet: ") + type, Logger::Category::Warning);
			}

			clientLastPacketIDs[type] = packetID;
		}
	}
}

unsigned long long NetworkObject::GetNetworkObjectID() const
{
	return networkObjectID;
}

void NetworkObject::ProcessReceivedData(const std::string& data, std::string& updateType, std::string& packetID, std::string& updateData)
{
	unsigned int i = 0;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			i++;
			break;
		}
		updateType += data[i];
	}

	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			i++;
			break;
		}
		packetID += data[i];
	}

	for (const auto& character : std::string(data.begin() + updateType.size() + packetID.size() + 2, data.end()))
	{
		updateData += character;
	}
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
	spawnedFromLocalSpawnRequest(false),
	networkObjectID(0U)
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
