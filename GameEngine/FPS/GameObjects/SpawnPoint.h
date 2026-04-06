#ifndef SPAWNPOINT_H
#define SPAWNPOINT_H

#include "GameEngine/GameObject/GameObject.h"

class BoundingSphereComponent;
class GraphicsObjectColored;

class SpawnPoint : public GameObject
{

public:

	SpawnPoint();

	~SpawnPoint();

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	void SetPosition(const glm::vec3&) override;

	void SetRotation(const glm::mat4&) override;

private:

	SpawnPoint(const SpawnPoint&) = delete;

	SpawnPoint& operator=(const SpawnPoint&) = delete;

	SpawnPoint(SpawnPoint&&) = delete;

	SpawnPoint& operator=(SpawnPoint&&) = delete;

	// Inherited via GameObject
	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void End() override;

	bool Hovered() const override;

	void Deserialize() override;

	BoundingSphereComponent* sphere;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	GraphicsObjectColored* arrow;
};

#endif // SPAWNPOINT_H