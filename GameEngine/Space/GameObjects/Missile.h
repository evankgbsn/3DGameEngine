#ifndef MISSILE_H
#define MISSILE_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

class Missile : public GameObject, public NetworkObject
{
public:

	Missile();

	~Missile();

private:

	Missile(const Missile&) = delete;

	Missile& operator=(const Missile&) = delete;

	Missile(Missile&&) = delete;

	Missile& operator=(Missile&&) = delete;

	// --- GameObject Overrides ---
	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetTransform() const override;

	// --- NetworkObject Overrides ---
	void OnServerSpawnConfirmation(const std::string& IP) override;

	void OnClientSpawnConfirmation() override;

	void OnSpawn() override;

	void OnDespawn() override;

	void OnDataReceived(const std::string& data) override;

};

#endif // MISSILE_H