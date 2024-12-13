#ifndef TREE_H
#define TREE_H

#include "GameEngine/GameObject/GameObject.h"

class GOTexturedLit;
class StaticCollider;

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

	GOTexturedLit* graphics;

	StaticCollider* collider;
};

#endif // TREE_H