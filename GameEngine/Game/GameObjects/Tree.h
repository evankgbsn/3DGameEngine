#ifndef TREE_H
#define TREE_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class StaticColliderComponent;

class Tree : public GameObject
{

public:

	Tree();

	~Tree();

	bool Hovered() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::vec3 GetPosition() const override;

	void Start() override;

	void End() override;

private:

	Tree(const Tree&) = delete;

	Tree& operator=(const Tree&) = delete;

	Tree(Tree&&) = delete;

	Tree& operator=(Tree&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void SetRotation(const glm::mat4& rotation) override;

	glm::mat4 GetRotation() const override;

	void Serialize() override;

	void Deserialize() override;

	GraphicsObjectTexturedLit* graphics;

	StaticColliderComponent* collider;
};

#endif // TREE_H