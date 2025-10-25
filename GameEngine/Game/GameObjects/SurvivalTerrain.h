#ifndef SURVIVALTERRAIN_H
#define SURVIVALTERRAIN_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class TerrainComponent;
class Sprite;

class SurvivalTerrain : public GameObject
{
public:

	SurvivalTerrain();

	~SurvivalTerrain();

private:

	SurvivalTerrain(const SurvivalTerrain&) = delete;

	SurvivalTerrain& operator=(const SurvivalTerrain&) = delete;

	SurvivalTerrain(SurvivalTerrain&&) = delete;

	SurvivalTerrain& operator=(SurvivalTerrain&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void SetupEditorCallbacks();

	void CleanupEditorCallbacks();

	TerrainComponent* terrain;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

};

#endif // SURVIVALTERRAIN_H