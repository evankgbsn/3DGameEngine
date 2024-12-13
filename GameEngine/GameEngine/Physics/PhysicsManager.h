#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include <PXConfig.h>
#include <PxPhysicsAPI.h>

class PhysicsManager
{

public:

	static void Initialize();

	static void Terminate();

private:

	friend class SingletonHelpers;

	PhysicsManager();

	~PhysicsManager();

	static PhysicsManager* instance;

	physx::PxDefaultErrorCallback defaultErrorCallback;

	physx::PxDefaultAllocator defaultAllocatorCallback;

	bool recordMemoryAllocations = true;

	physx::PxPhysics* physics;

	physx::PxFoundation* foundation;
};


#endif // PHYSICSMANAGER_H