#ifndef RIGIDBOX_H
#define RIGIDBOX_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class RigidBox : public GameObject
{
public:

	RigidBox();

	~RigidBox();

	void SetPosition(const glm::vec3& newPosition) override;

	glm::vec3 GetPosition() const override;

	void SetRotation(const glm::mat4& newRotation) override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	bool Hovered() const override;

private:

	RigidBox(const RigidBox&) = delete;

	RigidBox& operator=(const RigidBox&) = delete;

	RigidBox(RigidBox&&) = delete;

	RigidBox& operator=(RigidBox&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* rigidBody;

};

#endif // RIGIDBOX_H