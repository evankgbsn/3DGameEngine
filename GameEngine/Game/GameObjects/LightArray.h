#ifndef LIGHTARRAY_H
#define LIGHTARRAY_H

#include "GameEngine/GameObject/GameObject.h"

#include <vector>

class SpotLight;
class PointLight;

class LightArray : public GameObject
{

public:

	LightArray();

	~LightArray();

private:

	LightArray(const LightArray&) = delete;

	LightArray& operator=(const LightArray&) = delete;

	LightArray(LightArray&&) = delete;

	LightArray& operator=(LightArray&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	std::vector<SpotLight*> spotLights;

	PointLight* pointLight;
};



#endif // LIGHTTARRAY_H
