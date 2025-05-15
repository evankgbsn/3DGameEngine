#ifndef GAMETERRAIN_H
#define GAMETERRAIN_H

#include "GameEngine/GameObject/GameObject.h"

class TerrainComponent;

class GameTerrain : public GameObject
{
public:

	GameTerrain();

	~GameTerrain();

	TerrainComponent* GetTerrain() const;

private:

	GameTerrain(const GameTerrain&) = delete;

	GameTerrain& operator=(const GameTerrain&) = delete;

	GameTerrain(GameTerrain&&) = delete;

	GameTerrain& operator=(GameTerrain&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Serialize() override;

	void Deserialize() override;

	TerrainComponent* terrain;
};

#endif // GAMETERRAIN_H