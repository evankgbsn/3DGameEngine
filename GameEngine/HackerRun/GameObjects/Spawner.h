#ifndef SPAWNER_H
#define SPAWNER_H

#include "GameEngine/GameObject/GameObject.h"

#include <list>

class Corruption;
class HackerRunner;

class Spawner : public GameObject
{

public:

	Spawner();

	~Spawner();

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

	std::list<Corruption*> corruptionObjects;

	float corruptionSpawnInterval;

	float lastCorruptionSpawnTime;

	unsigned int maxCorruptionCount;
};

#endif