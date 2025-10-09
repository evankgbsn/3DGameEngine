#ifndef SURVIVALCHARACTER_H
#define SURVIVALCHARACTER_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

#include <functional>

class GraphicsObjectTexturedAnimatedLit;
class CameraComponent;
class AnimatedColliderComponent;

class SurvivalCharacter : public GameObject, public NetworkObject
{

public:

	SurvivalCharacter();

	~SurvivalCharacter();

private:

	SurvivalCharacter(const SurvivalCharacter&) = delete;

	SurvivalCharacter& operator=(const SurvivalCharacter&) = delete;

	SurvivalCharacter(SurvivalCharacter&&) = delete;

	SurvivalCharacter& operator=(SurvivalCharacter&&) = delete;

	void OnSpawn() override;

	void OnDespawn() override;

	void OnDataReceived(const std::string& data) override;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void Start() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Serialize() override;

	void Deserialize() override;

	bool Hovered() const override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	void SetPosition(const glm::vec3&) override;

	void SetRotation(const glm::mat4&) override;

	void SetupMovement();

	void CleanupMovement();

	void SetupCameraMovement();

	void CleanupCameraMovement();

	void MoveToTarget();

	void SetupEditorCallbacks();

	void CleanupEditorCallbacks();

	GraphicsObjectTexturedAnimatedLit* characterGraphics;

	CameraComponent* characterCamera;

	AnimatedColliderComponent* characterCollider;

	std::function<void(int)>* clickToMove;

	std::function<void(int)>* cameraOrbit;

	std::function<void(int)>* rotateCameraViewPress;

	std::function<void(int)>* rotateCameraViewRelease;

	std::function<void(double, double)>* zoomCamera;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	glm::vec3 target;

	float walkSpeed;

	glm::vec3 cameraTarget;

	glm::vec3 cameraPosition;

	float cameraDistance;

	glm::vec3 receivedPosition;

	glm::vec3 currentTranslationVector;

	std::function<void(const std::string&)>* onClientDisconnect;

};

#endif // SURVIVALCHARACTER_H