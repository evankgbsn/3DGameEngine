#include "PhysicsManager.h"

#include "../Utils/SingletonHelpers.h"
#include "../Utils/Logger.h"

#include <PxPhysicsAPI.h>

PhysicsManager* PhysicsManager::instance = nullptr;

void PhysicsManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "PhysicsManager");
}

void PhysicsManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "PhysicsManager");
}

PhysicsManager::PhysicsManager()
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	physx::PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	if (!mFoundation)
	{
		Logger::Log("PxCreateFoundation failed!", Logger::Category::Error);
	}
}

PhysicsManager::~PhysicsManager()
{
}
