#ifndef POINTLIGHTOBJECT_H
#define POINTLIGHTOBJECT_H

#include "GameEngine/GameObject/GameObject.h"

class PointLightComponent;

class PointLightObject : public GameObject
{

public:

	PointLightObject();

	~PointLightObject();

private:

	PointLightObject(const PointLightObject&) = delete;

	PointLightObject& operator=(const PointLightObject&) = delete;

	PointLightObject(PointLightObject&&) = delete;

	PointLightObject& operator=(PointLightObject&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void SetPosition(const glm::vec3& pos) override;

	glm::vec3 GetPosition() const override;

	void SetRotation(const glm::mat4& rot) override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	bool Hovered() const override;

	void Deserialize() override;

	void InitializeEditorCallbacks();

	void TerminateEditorCallbacks();

	PointLightComponent* light;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

};

#endif POINTLIGHTOBJECT_H