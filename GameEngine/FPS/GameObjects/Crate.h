#ifndef CRATE_H
#define CRATE_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class Crate : public GameObject
{

public:

	Crate();

	~Crate();

private:

	Crate(const Crate&) = delete;

	Crate& operator=(const Crate&) = delete;

	Crate(Crate&&) = delete;

	Crate& operator=(Crate&&) = delete;

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

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* body;
};

#endif // CRATE_H