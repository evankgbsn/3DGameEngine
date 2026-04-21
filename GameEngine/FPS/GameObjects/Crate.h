#ifndef CRATE_H
#define CRATE_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

#include <functional>

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class Crate : public GameObject, public NetworkObject
{

public:

	Crate();

	~Crate();

	void Hit(const glm::vec3& position, const glm::vec3& direction, float strength);

private:

	Crate(const Crate&) = delete;

	Crate& operator=(const Crate&) = delete;

	Crate(Crate&&) = delete;

	Crate& operator=(Crate&&) = delete;

	// Inherited via GameObject
	void OnServerSpawnConfirmation(const std::string& IP) override;

	void OnClientSpawnConfirmation() override;

	void OnSpawn() override;

	void OnDespawn() override;

	void OnDataReceived(const std::string& data) override;

	// Inherited via GameObject
	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	void SetPosition(const glm::vec3&) override;

	void SetRotation(const glm::mat4&) override;

	bool Hovered() const override;

	void Deserialize() override;

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* body;

	glm::mat4 rotationToSet;

	glm::vec3 positionToSet;

	float updateTime;

	float updateInterval;

	unsigned long long positionPacketNumber;

	unsigned long long rotationPacketNumber;

	std::function<void(const std::string&)>* positionReceivedCallback;

	std::function<void(const std::string&)>* rotationReceivedCallback;
};

#endif // CRATE_H