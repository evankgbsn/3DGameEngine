#ifndef SURVIVALCRATE_H
#define SURVIVALCRATE_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class GraphicsObjectTexturedLit;
class StaticColliderComponent;

class SurvivalCrate : public GameObject
{
public:

	SurvivalCrate();

	~SurvivalCrate();

	glm::vec3 GetPosition() const override;

private:

	SurvivalCrate(const SurvivalCrate&) = delete;

	SurvivalCrate& operator=(const SurvivalCrate&) = delete;

	SurvivalCrate(SurvivalCrate&&) = delete;

	SurvivalCrate& operator=(SurvivalCrate&&) = delete;

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

	GraphicsObjectTexturedLit* graphics;

	StaticColliderComponent* collider;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;
	
};

#endif // SURVIVALCRATE_H