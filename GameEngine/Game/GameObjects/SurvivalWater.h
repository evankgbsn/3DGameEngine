#ifndef SURVIVALWATER_H
#define SURVIVALWATER_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class TerrainComponent;

class SurvivalWater : public GameObject
{
public:

	SurvivalWater();

	~SurvivalWater();

private:

	SurvivalWater(const SurvivalWater&) = delete;

	SurvivalWater& operator=(const SurvivalWater&) = delete;

	SurvivalWater(SurvivalWater&&) = delete;

	SurvivalWater& operator=(SurvivalWater&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Serialize() override;

	void Deserialize() override;

	void SetupEditorCallbacks();

	void CleanupEditorCallbacks();

	TerrainComponent* waterTerrain;

	std::function<void()>* onEditorEnable;
	
	std::function<void()>* onEditorDisable;


};

#endif // SURVIVALWATER_H