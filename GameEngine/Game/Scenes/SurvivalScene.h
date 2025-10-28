#ifndef SURVIVALSCENE_H
#define SURVIVALSCENE_H

#include "GameEngine/Scene/Scene.h"

#include <functional>


class SurvivalCharacter;
class SurvivalSun;
class SurvivalTerrain;
class SurvivalTree;
class SurvivalWater;
class SurvivalServerFreeCamera;
class SurvivalCrate;
class SurvivalPointLight;
class SurvivalRockLarge;

class SurvivalScene : public Scene
{
public:

	SurvivalScene();

	~SurvivalScene();

	void Initialize() override;

	void Terminate() override;

private:

	SurvivalScene(const SurvivalScene&) = delete;

	SurvivalScene& operator=(const SurvivalScene&) = delete;

	SurvivalScene(SurvivalScene&&) = delete;

	SurvivalScene& operator=(SurvivalScene&&) = delete;

	void Load() override;

	void Unload() override;

	SurvivalCharacter* player;

	SurvivalSun* sun;

	SurvivalTree* tree;

	SurvivalTerrain* terrain;

	SurvivalWater* water;

	SurvivalServerFreeCamera* serverFreeCam;

	SurvivalCrate* crate;

	SurvivalPointLight* pointLight;

	SurvivalRockLarge* largeRock;

};

#endif // SURVIVALSCENE_H