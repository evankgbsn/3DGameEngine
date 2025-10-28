#ifndef SURVIVALTREE_H
#define SURVIVALTREE_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class GraphicsObjectTexturedLit;
class StaticColliderComponent;

class SurvivalTree : public GameObject
{
public:

	SurvivalTree();

	~SurvivalTree();

	glm::vec3 GetPosition() const override;

private:

	SurvivalTree(const SurvivalTree&) = delete;

	SurvivalTree& operator=(const SurvivalTree&) = delete;

	SurvivalTree(SurvivalTree&&) = delete;

	SurvivalTree& operator=(SurvivalTree&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void SetPosition(const glm::vec3& newPosition) override;

	void SetRotation(const glm::mat4& newRotation) override;

	glm::mat4 GetRotation() const override;

	bool Hovered() const override;

	void SetupEditorCallbacks();

	void CleanupEditorCallbacks();

	void Deserialize() override;

	GraphicsObjectTexturedLit* trunkGraphics;

	GraphicsObjectTexturedLit* branchesGraphics;

	GraphicsObjectTexturedLit* leavesGraphics;

	StaticColliderComponent* collider;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

};

#endif // SURVIVALTREE_H