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

private:

	RigidBox(const RigidBox&) = delete;

	RigidBox& operator=(const RigidBox&) = delete;

	RigidBox(RigidBox&&) = delete;

	RigidBox& operator=(RigidBox&&) = delete;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void SetPosition(const glm::vec3& newPosition) override;

	glm::vec3 GetPosition() const override;

	void SetRotation(const glm::mat4& newRotation) override;

	glm::mat4 GetRotation() const override;

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* rigidBody;

};

#endif // RIGIDBOX_H