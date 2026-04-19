#ifndef MAINDIRECTIONALLIGHT_H
#define MAINDIRECTIONALLIGHT_H

#include "GameEngine/GameObject/GameObject.h"

class DirectionalLightComponent;

class MainDirectionalLight : public GameObject
{
public:

	MainDirectionalLight();

	~MainDirectionalLight();

private:

	MainDirectionalLight(const MainDirectionalLight&) = delete;

	MainDirectionalLight& operator=(const MainDirectionalLight&) = delete;

	MainDirectionalLight(MainDirectionalLight&&) = delete;

	MainDirectionalLight& operator=(MainDirectionalLight&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetTransform() const override;

	DirectionalLightComponent* light;

};

#endif // MAINDIRECTIONALLIGHT_H