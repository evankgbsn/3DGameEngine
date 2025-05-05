#ifndef DIRECTIONALLIGHTGAMEOBJECT_H
#define DIRECTIONALLIGHTGAMEOBJECT_H

#include "GameEngine/GameObject/GameObject.h"

class DirectionalLight;

class DirectionalLightGameObject : public GameObject
{

public:

	DirectionalLightGameObject();

	~DirectionalLightGameObject();

private:

	DirectionalLightGameObject(const DirectionalLightGameObject&) = delete;

	DirectionalLightGameObject& operator=(const DirectionalLightGameObject&) = delete;

	DirectionalLightGameObject(DirectionalLightGameObject&&) = delete;

	DirectionalLightGameObject& operator=(DirectionalLightGameObject&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	void SetPosition(const glm::vec3& pos) override;

	glm::vec3 GetPosition() const override;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	DirectionalLight* light;

};

#endif // DIRECTIONALLIGHTGAMEOBJECT_H