#ifndef PLAYERSHIP_H
#define PLAYERSHIP_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

#include <unordered_map>
#include <functional>

class CameraComponent;
class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class PlayerShip : public GameObject, public NetworkObject
{
public:

	PlayerShip();

	~PlayerShip();

private:

	PlayerShip(const PlayerShip&) = delete;

	PlayerShip& operator=(const PlayerShip&) = delete;

	PlayerShip(PlayerShip&&) = delete;

	PlayerShip& operator=(PlayerShip&&) = delete;

	void Initialize() override;

	void InitializeLocalPlayer();

	void InitializeServer();

	void InitializeRemotePlayer();

	void Terminate() override;

	void TerminateLocalPlayer();

	void TerminateServer();

	void TerminateRemotePlayer();

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void OnSpawn() override;

	void OnDespawn() override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& pos) override;

	glm::mat4 GetRotation() const override;
	
	void SetRotation(const glm::mat4& rot) override;

	glm::mat4 GetTransform() const override;

	void OnDataReceived(const std::string& data) override;

	void OnServerSpawnConfirmation(const std::string& IP) override;

	void OnClientSpawnConfirmation() override;

	void OrientCamera();

	void RegisterInput();

	void DeregisterInput();

	void AddClientDataReceivedCallbacks();

	void AddServerDataReceivedCallbacks();

	void SendServerPositionUpdates();

	void UpdatePhysics();

	GraphicsObjectTexturedLit* graphics;

	CameraComponent* cam;

	RigidBodyComponent* body;

	glm::vec3 camOffset;

	std::unordered_map<std::string, std::function<void(const std::string&)>*> clientDataReceivedCallbacks;

	std::unordered_map<std::string, std::function<void(const std::string&)>*> serverDataReceivedCallbacks;

	std::function<void(int)>* move;

	std::function<void(const glm::vec2&)>* look;

	unsigned long long movePacketNumber = 0;

	unsigned long long lookPacketNumber = 0;

	unsigned long long positionPacketNumber = 0;

	float speed;

	float positionUpdateInterval;

	std::function<void(const std::string&)>* onClientDisconnect;

};

#endif // PLAYERSHIP_H