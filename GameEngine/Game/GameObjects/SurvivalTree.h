#ifndef SURVIVALTREE_H
#define SURVIVALTREE_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>
#include <list>

class GraphicsObjectTexturedLitInstanced;
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

	void Serialize() override;

	void Deserialize() override;

	bool Hovered() const override;

	void SetPosition(const glm::vec3& newPos) override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetRotation() const override;

	void SetupEditorCallbacks();

	void CleanupEditorCallbacks();

	static GraphicsObjectTexturedLitInstanced* trunk;
	static GraphicsObjectTexturedLitInstanced* branches;
	static GraphicsObjectTexturedLitInstanced* leaves;

	StaticColliderComponent* collider;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	unsigned int instanceID;

	static unsigned int instanceIDGen;

	static std::list<std::function<void()>*> onGraphicsDeserializedFunctions;

};

#endif // SURVIVALTREE_H