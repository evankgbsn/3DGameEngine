#ifndef GROUND_H
#define GROUND_h

#include "GameEngine/GameObject/GameObject.h"

class TerrainComponent;
class RigidBodyComponent;

class Ground : public GameObject
{

public:

	Ground();

	~Ground();

private:

	Ground(const Ground&) = delete;

	Ground& operator=(const Ground&) = delete;

	Ground(Ground&&) = delete;

	Ground& operator=(Ground&&) = delete;

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

	TerrainComponent* terrain;

	RigidBodyComponent* body;
};

#endif // GROUND_H