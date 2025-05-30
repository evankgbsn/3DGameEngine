#ifndef FPSTEST_H
#define FPSTEST_H

#include "GameEngine/Scene/Scene.h"

#include <functional>

class LargePlane;
class DirectionalLightGameObject;
class Player;
class RigidBox;

class FPSTest : public Scene
{
public:

	FPSTest();

	~FPSTest();

private:

	FPSTest(const FPSTest&) = delete;

	FPSTest& operator=(const FPSTest&) = delete;

	FPSTest(FPSTest&&) = delete;

	FPSTest& operator=(FPSTest&&) = delete;

	void Initialize() override;

	void Terminate() override;

	LargePlane* terrain;

	DirectionalLightGameObject* light;

	Player* player;

	RigidBox* box;

	std::function<void(int)>* deserialize;

};


#endif // FPSTEST_H