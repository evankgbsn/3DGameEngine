#ifndef SURVIVALROCKLARGE_H
#define SURVIVALROCKLARGE_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class GraphicsObjectTexturedLit;
class OrientedBoundingBoxComponent;

class SurvivalRockLarge : public GameObject
{
public:

	SurvivalRockLarge();

	~SurvivalRockLarge();

private:

	SurvivalRockLarge(const SurvivalRockLarge&) = delete;

	SurvivalRockLarge& operator=(const SurvivalRockLarge&) = delete;

	SurvivalRockLarge(SurvivalRockLarge&&) = delete;

	SurvivalRockLarge& operator=(SurvivalRockLarge&&) = delete;

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

	glm::vec3 GetPosition() const override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetRotation() const override;

	void SetupEditorCallbacks();

	void CleanupEditorCallbacks();

	GraphicsObjectTexturedLit* graphics;

	OrientedBoundingBoxComponent* collider;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

};

#endif // SURVIVALROCKLARGE_H