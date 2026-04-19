#include "Missile.h"

Missile::Missile() :
	GameObject("Missile"),
	NetworkObject()
{
	// Registers this class type for network instantiation
	RegisterNetworkObjectClassType<Missile>(this);
}

Missile::~Missile()
{
}

// --- GameObject Implementation ---

void Missile::Initialize()
{
}

void Missile::Terminate()
{
}

void Missile::GameUpdate()
{
}

void Missile::EditorUpdate()
{
}

void Missile::Load()
{
}

void Missile::Unload()
{
}

bool Missile::Hovered() const
{
	return false;
}

glm::vec3 Missile::GetPosition() const
{
	return glm::vec3();
}

void Missile::SetPosition(const glm::vec3& newPos)
{
}

glm::mat4 Missile::GetRotation() const
{
	return glm::mat4();
}

void Missile::SetRotation(const glm::mat4& newRot)
{
}

glm::mat4 Missile::GetTransform() const
{
	return glm::mat4();
}

// --- NetworkObject Implementation ---

void Missile::OnServerSpawnConfirmation(const std::string& IP)
{
	NetworkObject::OnServerSpawnConfirmation(IP);
}

void Missile::OnClientSpawnConfirmation()
{
}

void Missile::OnSpawn()
{
	// Base OnSpawn handles registering data receive functions
	NetworkObject::OnSpawn();
}

void Missile::OnDespawn()
{
	// Base OnDespawn handles deregistering data receive functions
	NetworkObject::OnDespawn();
}

void Missile::OnDataReceived(const std::string& data)
{
	// Base OnDataReceived handles packet ordering and callbacks
	NetworkObject::OnDataReceived(data);
}