#ifndef PLAYERSHIP_H
#define PLAYERSHIP_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

class CameraComponent;
class GraphicsObjectTexturedLit;

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

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void OnSpawn() override;

	void OnDespawn() override;

	void OnDataReceived(const std::string& data) override;

	void OnServerSpawnConfirmation(const std::string& IP) override;

	void OnClientSpawnConfirmation() override;

	GraphicsObjectTexturedLit* graphics;

	CameraComponent* cam;

};

#endif // PLAYERSHIP_H