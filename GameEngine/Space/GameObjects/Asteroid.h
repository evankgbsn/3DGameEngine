#ifndef ASTEROID_H
#define ASTEROID_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class Asteroid : public GameObject
{
public:

	Asteroid();

	~Asteroid();

private:

	Asteroid(const Asteroid&) = delete;

	Asteroid& operator=(const Asteroid&) = delete;

	Asteroid(Asteroid&&) = delete;

	Asteroid& operator=(Asteroid&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetTransform() const override;

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* body;

};

#endif // ASTEROID_H