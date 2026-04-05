#ifndef SPOTLIGHTOBJECT_H
#define SPOTLIGHTOBJECT_H

#include "GameEngine/GameObject/GameObject.h"

class SpotLightComponent;

class SpotLightObject : public GameObject
{

public:

	SpotLightObject();

	~SpotLightObject();

private:

	SpotLightObject(const SpotLightObject&) = delete;

	SpotLightObject& operator=(const SpotLightObject&) = delete;

	SpotLightObject(SpotLightObject&&) = delete;

	SpotLightObject& operator=(SpotLightObject&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void SetPosition(const glm::vec3& pos) override;

	glm::vec3 GetPosition() const override;

	void SetRotation(const glm::mat4& rot) override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	bool Hovered() const override;

	void Deserialize() override;

	void InitializeEditorCallbacks();

	void TerminateEditorCallbacks();

	SpotLightComponent* light;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

};

#endif // SPOTLIGHTOBJECT_H