#ifndef FPSPLAYER_H
#define FPSPLAYER_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

#include <functional>
#include <atomic>

class GraphicsObjectTexturedAnimatedLit;
class RigidBodyComponent;
class CameraComponent;
class AnimatedColliderComponent;
class CharacterControllerComponent;
class GraphicsObjectTexturedLit;
class Sprite;
class GraphicsObjectLine;

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

	glm::mat4 ConvertRightHandTransformToWeaponTransform() const;

	void Shoot(float rewindTime);

	void GetSpawnPositionAndRotation(glm::vec3& position, glm::mat4& rotation);

	GraphicsObjectTexturedAnimatedLit* characterGraphics;

	GraphicsObjectTexturedAnimatedLit* characterArmsGraphics;

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

	std::function<void(int axis, float value)>* gamepadShoot;

	std::function<void(int button)>* gamepadJump;

	std::function<void(int key)>* keyboardMove;

	std::function<void(int key)>* keyboardJump;

	std::function<void(int button)>* keyboardShoot;

	std::function<void(int button)>* keyboardShootRelease;

	std::function<void(const std::string&)>* onClientDisconnect;

	std::function<void(int key)>* keyboardMovePress;

	std::function<void(int key)>* keyboardMoveRelease;

	bool wPressed = false;

	bool aPressed = false;

	bool sPressed = false;

	bool dPressed = false;

	bool ignoreServerPos = false;

	glm::vec3 disp;

	glm::vec3 positionToSet = { 0.0f, 20.0f, 0.0f };

	glm::vec3 footPositionToSet = { 0.0f, 20.0f, 0.0f };

	glm::vec3 targetToSet;

	glm::mat4 weaponPositionToSet;

	float additiveUpToSet;

	float additiveDownToSet;

	unsigned int positionPacketNumber = 0;

	unsigned int footPositionPacketNumber = 0;

	unsigned int animationClipPacketNumber = 0;

	unsigned int lastPositionPacketNumber = 0;

	unsigned int lastFootPositionPacketNumber = 0;

	unsigned int targetPacketNumber = 0;

	unsigned int lastTargetPacketNumber = 0;

	unsigned int dispPacketNumber = 0;

	unsigned int lastDispPacketNumber = 0;

	unsigned int jumpPacketNumber = 0;

	unsigned int shootPacketNumber = 0;

	unsigned int lastJumpPacketNumber = 0;

	unsigned int lastShootPacketNumber = 0;

	unsigned int weaponPositionPacketNumber = 0;

	unsigned int additiveAnimationUpPacketNumber = 0;

	unsigned int additiveAnimationDownPacketNumber = 0;

	unsigned int lookUpPacketNumber = 0;

	unsigned int lookDownPacketNumber = 0;

	unsigned int damagePacketNumber = 0;

	unsigned int lastWeaponPositionPacketNumber = 0;

	unsigned int lastAdditiveAnimationUpPacketNumber = 0;

	unsigned int lastAdditiveAnimationDownPacketNumber = 0;

	unsigned int lastAnimationClipPacketNumber = 0;

	unsigned int lastDamagePacketNumber = 0;

	float lastShotTime = 0.0f;

	float updateTime = 0.0f;

	float camUpdateTime = 0.0f;

	Sprite* crosshair;

	glm::vec3 lastPosition;

	glm::vec3 lastTarget;

	std::atomic<bool> newPositionFromServer = false;

	std::atomic<bool> newFootPositionFromServer = false;

	GraphicsObjectLine* shotCast;

	std::function<void(const std::string&)>* animationStartCallback;

	std::function<void(const std::string&)>* animationStopCallback;

	float health = 100.0f;

	Sprite* healthBar;

	bool shouldDie = false;

	bool walking = false;

	bool shot = false;

	glm::vec3 spawnTarget;

	float jumpPower = 15.0f;

	float timeBetweenShots;

	float recoilTime;

};

#endif // FPSPLAYER_H