#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include <PXConfig.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

using namespace physx;

class PhysicsManager
{

public:

	static void Initialize();

	static void Terminate();

	static void Update();

	static physx::PxPhysics* const GetPhysics();

	static physx::PxScene* const GetScene();

	static bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, PxRaycastBuffer& outHit);

	static bool RaycastFromCursor(float maxDistance, PxRaycastBuffer& outHit);

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