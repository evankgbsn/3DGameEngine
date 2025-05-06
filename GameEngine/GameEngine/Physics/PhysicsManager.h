#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include <PXConfig.h>
#include <PxPhysicsAPI.h>

using namespace physx;

class PhysicsManager
{

public:

	static void Initialize();

	static void Terminate();

	static void Update();

	static physx::PxPhysics* const GetPhysics();

	static physx::PxScene* const GetScene();

private:

	friend class SingletonHelpers;

	PhysicsManager();

	~PhysicsManager();

	static PhysicsManager* instance;

	PxDefaultErrorCallback defaultErrorCallback;

	PxDefaultAllocator defaultAllocatorCallback;

	bool recordMemoryAllocations = true;

	PxPhysics* physics;

	PxFoundation* foundation;

	PxScene* scene;

	PxDefaultCpuDispatcher* dispatcher;

	float accumulator;

	float stepSize;
};


#endif // PHYSICSMANAGER_H