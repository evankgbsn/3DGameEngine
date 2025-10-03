#ifndef SURVIVALSUN_H
#define SURVIVALSUN_H

#include "GameEngine/GameObject/GameObject.h"

class DirectionalLightComponent;

class SurvivalSun : public GameObject
{

public:

	SurvivalSun();

	~SurvivalSun();

private:

	SurvivalSun(const SurvivalSun&) = delete;

	SurvivalSun& operator=(const SurvivalSun&) = delete;

	SurvivalSun(SurvivalSun&&) = delete;

	SurvivalSun& operator=(SurvivalSun&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	DirectionalLightComponent* sun;
};


#endif // SURVIVALSUN_H