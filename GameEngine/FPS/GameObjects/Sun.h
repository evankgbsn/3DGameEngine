#ifndef SUN_H
#define SUN_H

#include "GameEngine/GameObject/GameObject.h"

class DirectionalLightComponent;

class Sun : public GameObject
{

public:

	Sun();

	~Sun();

private:

	Sun(const Sun&) = delete;

	Sun& operator=(const Sun&) = delete;

	Sun(const Sun&&) = delete;

	Sun& operator=(const Sun&&) = delete;

	// Inherited via GameObject
	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	void SetPosition(const glm::vec3&) override;

	void SetRotation(const glm::mat4&) override;

	bool Hovered() const override;

	DirectionalLightComponent* light;
};

#endif // SUN_H