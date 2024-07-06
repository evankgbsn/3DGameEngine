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

	DirectionalLight* light;

};

#endif // DIRECTIONALLIGHTGAMEOBJECT_H