#ifndef PHYSXTEST_H
#define PHYSXTEST_H

#include "GameEngine/Scene/Scene.h"

class LargePlane;
class DirectionalLightGameObject;
class FreeCamera;
class RigidBox;

class PhysXTest : public Scene
{
public:

	PhysXTest();

	~PhysXTest();

private:

	PhysXTest(const PhysXTest&) = delete;

	PhysXTest& operator=(const PhysXTest&) = delete;

	PhysXTest(PhysXTest&&) = delete;

	PhysXTest& operator=(PhysXTest&&) = delete;

	void Initialize() override;

	void Terminate() override;

	LargePlane* groundPlane;

	DirectionalLightGameObject* light;

	FreeCamera* cam;

	RigidBox* box;

};

#endif // PHYSXTEST_H