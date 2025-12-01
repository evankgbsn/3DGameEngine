#ifndef TEST_H
#define TEST_H

#include "GameEngine/Scene/Scene.h"

class PlaneObj;
class FPSPlayer;
class Sun;
class Crate;
class Ground;
class ServerFreeCamera;

class Test : public Scene
{

public:

	Test();

	~Test();

private:

	Test(const Test&) = delete;

	Test& operator=(const Test&) = delete;

	Test(Test&&) = delete;

	Test& operator=(Test&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	FPSPlayer* player;
	Sun* sun;
	Crate* crate;
	Ground* ground;
	ServerFreeCamera* serverFreeCam;
};

#endif // TEST_H