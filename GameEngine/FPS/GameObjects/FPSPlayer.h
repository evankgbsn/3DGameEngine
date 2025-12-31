#ifndef FPSPLAYER_H
#define FPSPLAYER_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

#include <functional>

class GraphicsObjectTexturedAnimatedLit;
class RigidBodyComponent;
class CameraComponent;
class AnimatedColliderComponent;
class CharacterControllerComponent;
class GraphicsObjectTexturedLit;

class FPSPlayer : public GameObject, public NetworkObject
{

public:

	FPSPlayer();

	~FPSPlayer();

	bool IsLocal() const;

	glm::mat4 GetWeaponTransform();

private:

	FPSPlayer(const FPSPlayer&) = delete;

	FPSPlayer& operator=(const FPSPlayer&) = delete;

	FPSPlayer(FPSPlayer&&) = delete;

	FPSPlayer& operator=(FPSPlayer&&) = delete;

	void OnSpawn() override;

	void OnDespawn() override;

	void OnDataReceived(const std::string& data) override;

	void OnServerSpawnConfirmation(const std::string& IP) override;

	void OnClientSpawnConfirmation() override;

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

	void Start() override;

	void RegisterInput();

	void DeregisterInput();

	void RegisterEditorToggleCallbacks();

	void DeregisterEditorToggleCallbacks();

	GraphicsObjectTexturedAnimatedLit* characterGraphics;

	GraphicsObjectTexturedLit* ak12Graphics;

	CameraComponent* cam;

	AnimatedColliderComponent* hitBox;

	CharacterControllerComponent* controller;

	std::function<void(const glm::vec2&)>* whenCursorMove;

	std::function<void()>* editorEnable;
	
	std::function<void()>* editorDisable;

	std::function<void(int axis, float value)>* gamepadLookX;

	std::function<void(int axis, float value)>* gamepadLookY;

	std::function<void(int axis, float value)>* gamepadWalkX;

	std::function<void(int axis, float value)>* gamepadWalkY;

	std::function<void(int button)>* gamepadJump;

	std::function<void(int key)>* keyboardMove;

	std::function<void(int key)>* keyboardJump;

	std::function<void(int button)>* keyboardShoot;

	std::function<void(const std::string&)>* onClientDisconnect;

	std::function<void(int key)>* keyboardMovePress;

	std::function<void(int key)>* keyboardMoveRelease;

	bool wPressed = false;

	bool aPressed = false;

	bool sPressed = false;

	bool dPressed = false;

	glm::vec3 disp;

	glm::vec3 positionToSet;

	glm::vec3 targetToSet;

	glm::mat4 weaponPositionToSet;

	unsigned int positionPacketNumber = 0;

	unsigned int lastPositionPacketNumber = 0;

	unsigned int targetPacketNumber = 0;

	unsigned int lastTargetPacketNumber = 0;

	unsigned int dispPacketNumber = 0;

	unsigned int lastDispPacketNumber = 0;

	unsigned int jumpPacketNumber = 0;

	unsigned int lastJumpPacketNumber = 0;

	unsigned int shootPacketNumber = 0;

	unsigned int lastShootPacketNumber = 0;

	unsigned int weaponPositionPacketNumber = 0;

	unsigned int lastWeaponPositionPacketNumber = 0;

	float lastShotTime = 0.0f;

	float updateTime = 0.0f;

};

#endif // FPSPLAYER_H