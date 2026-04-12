#ifndef HACKERRUNMAIN_H
#define HACKERRUNMAIN_H

#include "GameEngine/Scene/Scene.h"

class HackerRunner;
class Corruption;
class Spawner;

class HackerRunMain : public Scene
{

public:

	HackerRunMain();

	~HackerRunMain();

private:

	HackerRunMain(const HackerRunMain&) = delete;

	HackerRunMain& operator=(const HackerRunMain&) = delete;

	HackerRunMain(HackerRunMain&&) = delete;

	HackerRunMain& operator=(HackerRunMain&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void End() override;

	HackerRunner* player;

	Corruption* corruption;

	Spawner* spawner;
};

#endif