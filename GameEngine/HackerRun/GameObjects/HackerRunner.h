#ifndef HACKERRUNNER_H
#define HACKERRUNNER_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class CameraComponent;
class GraphicsObjectColored;
class RigidBodyComponent;
class Text3D;

class HackerRunner : public GameObject
{

public:

	HackerRunner();

	~HackerRunner();

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& newRot) override;

	float GetSpeed() const;

private:

	HackerRunner(const HackerRunner&) = delete;

	HackerRunner& operator=(const HackerRunner&) = delete;

	HackerRunner(HackerRunner&&) = delete;

	HackerRunner& operator=(HackerRunner&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void End() override;

	void RegisterInput();

	void DeregisterInput();

	void Die();

	std::function<void(int)>* keyPress;

	std::function<void(int)>* mouseClick;

	std::function<void(int)>* keyRelease;

	std::function<void(const glm::vec2&)>* mouseMove;

	std::function<void(GameObject*)>* corruptionCollisionCallback;

	GraphicsObjectColored* graphics;

	CameraComponent* cam;

	glm::vec3 camOffset;

	float speed;

	float speedCap;

	RigidBodyComponent* body;

	bool rotateRight = true;

	bool rotateUp = true;

	float mouseSense;

	float yaw;

	float pitch;

	Text3D* binary;

};

#endif