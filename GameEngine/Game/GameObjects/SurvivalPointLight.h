#ifndef SURVIVALPOINTLIGHT_H
#define SURVIVALPOINTLIGHT_H

#include "GameEngine/GameObject/GameObject.h"

class PointLightComponent;

class SurvivalPointLight : public GameObject
{
public:

	SurvivalPointLight();

	~SurvivalPointLight();

private:

	SurvivalPointLight(const SurvivalPointLight&) = delete;

	SurvivalPointLight& operator=(const SurvivalPointLight&) = delete;

	SurvivalPointLight(SurvivalPointLight&&) = delete;

	SurvivalPointLight& operator=(SurvivalPointLight&&) = delete;

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

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	PointLightComponent* light;
};

#endif // SURVIVALPOINTLIGHT_H