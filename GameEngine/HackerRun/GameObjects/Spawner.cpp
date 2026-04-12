#include "Spawner.h"

#include "GameEngine/Scene/Scene.h"
#include "GameEngine/Time/TimeManager.h"

#include "Corruption.h"

Spawner::Spawner() :
	GameObject("Spawner"),
	corruptionSpawnInterval(3.0f),
	maxCorruptionCount(15U)
{
}

Spawner::~Spawner()
{
	for (Corruption* c : corruptionObjects)
	{
		c->GetOwningScene()->DeregisterGameObject(c->GetName(), [](GameObject* obj) {delete static_cast<Corruption*>(obj);});
	}
}

void Spawner::Initialize()
{
}

void Spawner::Terminate()
{
}

void Spawner::GameUpdate()
{
	if (TimeManager::SecondsSinceStart() - lastCorruptionSpawnTime > corruptionSpawnInterval && corruptionObjects.size() < maxCorruptionCount)
	{
		Corruption* newCorruption = new Corruption();
		GetOwningScene()->RegisterGameObject(newCorruption, std::to_string((int)newCorruption));

		lastCorruptionSpawnTime = TimeManager::SecondsSinceStart();
	}
}

void Spawner::EditorUpdate()
{
}

void Spawner::Load()
{
}

void Spawner::Unload()
{
}
