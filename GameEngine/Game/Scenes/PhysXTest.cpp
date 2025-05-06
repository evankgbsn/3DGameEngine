#include "PhysXTest.h"

#include "../GameObjects/LargePlane.h"
#include "../GameObjects/DirectionalLightGameObject.h"
#include "../GameObjects/FreeCamera.h"
#include "../GameObjects/RigidBox.h"

PhysXTest::PhysXTest() :
	groundPlane(new LargePlane()),
	light(new DirectionalLightGameObject()),
	cam(new FreeCamera()),
	box(new RigidBox())
{
}

PhysXTest::~PhysXTest()
{
	delete box;
	delete cam;
	delete light;
	delete groundPlane;
}

void PhysXTest::Initialize()
{
	RegisterGameObject(groundPlane, "Floor");
	RegisterGameObject(light, "Light");
	RegisterGameObject(cam, "Camera");
	RegisterGameObject(box, "RigidBox");
}

void PhysXTest::Terminate()
{
	DeregisterGameObject("RigidBox");
	DeregisterGameObject("Camera");
	DeregisterGameObject("Light");
	DeregisterGameObject("Floor");
}
