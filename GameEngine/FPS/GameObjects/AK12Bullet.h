#ifndef AK12BULLET_H
#define AK12BULLET_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

class GraphicsObjectTexturedLit;
class OrientedBoundingBoxComponent;

class AK12Bullet : public GameObject, public NetworkObject
{

public:

	AK12Bullet();

	~AK12Bullet();

private:

	AK12Bullet(const AK12Bullet&) = delete;

	AK12Bullet& operator=(const AK12Bullet&) = delete;

	AK12Bullet(AK12Bullet&&) = delete;

	AK12Bullet& operator=(AK12Bullet&&) = delete;

	// Inherited via NetworkObject
	void OnSpawn() override;

	void OnDespawn() override;

	void OnDataReceived(const std::string& data) override;

	void OnServerSpawnConfirmation(const std::string& IP) override;

	void OnClientSpawnConfirmation() override;

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

	OrientedBoundingBoxComponent* collider;

	glm::vec3 positionToSet;

	unsigned int positionPacketNumber = 0;

	float speed;

	
};

#endif // AK12BULLET_H