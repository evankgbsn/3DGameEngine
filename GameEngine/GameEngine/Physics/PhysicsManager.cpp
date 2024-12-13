#include "PhysicsManager.h"

#include "../Utils/SingletonHelpers.h"
#include "../Utils/Logger.h"

PhysicsManager* PhysicsManager::instance = nullptr;

void PhysicsManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "PhysicsManager");
}

void PhysicsManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "PhysicsManager");
}

PhysicsManager::PhysicsManager() :
	recordMemoryAllocations(true),
	defaultAllocatorCallback(),
	defaultErrorCallback(),
	physics(nullptr)
{
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
	
	if (!foundation)
	{
		Logger::Log("PxCreateFoundation failed!", Logger::Category::Error);
	}
	else
	{
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), recordMemoryAllocations, nullptr);
		
		if (!physics)
		{
			Logger::Log("PxCreatePhysics failed!", Logger::Category::Error);
		}
	}
}

PhysicsManager::~PhysicsManager()
{
	physics->release();
	foundation->release();
}
