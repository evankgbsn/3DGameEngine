#ifndef SPAWNER_H
#define SPAWNER_H

#include "GameEngine/GameObject/GameObject.h"

#include <list>

class Corruption;
class HackerRunner;
class MemoryBlock;
class GraphicsObjectLine;

class Spawner : public GameObject
{

public:

	Spawner();

	~Spawner();

	const std::vector<std::vector<MemoryBlock*>>& GetBlocks() const;

	float GetSpaceBetweenBlocks() const;

private:

	Spawner(const Spawner&) = delete;

	Spawner& operator=(const Spawner&) = delete;

	Spawner(Spawner&&) = delete;

	Spawner& operator=(Spawner&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void CreateInitialMemoryBlocks();

	std::list<Corruption*> corruptionObjects;

	float corruptionSpawnInterval;

	float lastCorruptionSpawnTime;

	unsigned int maxCorruptionCount;

	std::vector<std::vector<MemoryBlock*>> blocks;

	std::list<GraphicsObjectLine*> dataLines;

	glm::ivec2 blockPos;

	float spaceBetweenBlocks;

	int radius;

	float lineDataSpeed;

	int forwardSpawnOffset;

	bool hasReachedGrid;
};

#endif