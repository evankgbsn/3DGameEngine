#include "PhysicsManager.h"

#include "../Utils/SingletonHelpers.h"
#include "../Utils/Logger.h"

#include <PXConfig.h>
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
	bool recordMemoryAllocations = true;
	
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;
	
	physx::PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	
	if (!mFoundation)
	{
		Logger::Log("PxCreateFoundation failed!", Logger::Category::Error);
	}
	else
	{
		physx::PxPhysics* mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, nullptr);
		
		if (!mPhysics)
		{
			Logger::Log("PxCreatePhysics failed!", Logger::Category::Error);
		}
	}

	
}

PhysicsManager::~PhysicsManager()
{
}
