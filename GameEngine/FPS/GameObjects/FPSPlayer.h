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

class FPSPlayer : public GameObject, public NetworkObject
{

public:

	FPSPlayer();

	~FPSPlayer();

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

	GraphicsObjectTexturedAnimatedLit* characterGraphics;

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

	std::function<void(const std::string&)>* onClientDisconnect;

	glm::vec3 disp;

};

#endif // FPSPLAYER_H