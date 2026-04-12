#ifndef HACKERRUNNER_H
#define HACKERRUNNER_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class CameraComponent;
class GraphicsObjectColored;
class RigidBodyComponent;

class HackerRunner : public GameObject
{

public:

	HackerRunner();

	~HackerRunner();

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

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	void RegisterInput();

	void DeregisterInput();

	void Die();

	std::function<void(int)>* keyPress;

	std::function<void(GameObject*)>* corruptionCollisionCallback;

	GraphicsObjectColored* graphics;

	CameraComponent* cam;

	glm::vec3 camOffset;

	float speed;

	float strafeSpeed;

	float strafeSpeedIncreaseInterval;

	RigidBodyComponent* body;

};

#endif