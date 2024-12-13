#ifndef LIGHTOBJECT_H
#define LIGHTOBJECT_H

#include "GameEngine/GameObject/GameObject.h"

#include <vector>

class SpotLight;

class LightObject : public GameObject
{

public:

	LightObject();

	~LightObject();

private:

	LightObject(const LightObject&) = delete;

	LightObject& operator=(const LightObject&) = delete;

	LightObject(LightObject&&) = delete;

	LightObject& operator=(LightObject&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	void SetPosition(const glm::vec3& pos) override;

	glm::vec3 GetPosition() const override;

	SpotLight* spotLight;
};



#endif // LIGHTTARRAY_H
