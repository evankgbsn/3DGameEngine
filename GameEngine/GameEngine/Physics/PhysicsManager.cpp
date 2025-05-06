#include "PhysicsManager.h"

#include "../Utils/SingletonHelpers.h"
#include "../Utils/Logger.h"
#include "../Time/TimeManager.h"

PhysicsManager* PhysicsManager::instance = nullptr;

void PhysicsManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "PhysicsManager");
}

void PhysicsManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "PhysicsManager");
}

void PhysicsManager::Update()
{
	if (instance != nullptr)
	{
		instance->accumulator += TimeManager::DeltaTime();
		if (instance->accumulator < instance->stepSize)
			return;

		instance->accumulator -= instance->stepSize;

		instance->scene->simulate(instance->stepSize);

		instance->scene->fetchResults(true);
	}
}

physx::PxPhysics* const PhysicsManager::GetPhysics()
{
	if (instance != nullptr)
	{
		return instance->physics;
	}

	return nullptr;
}

physx::PxScene* const PhysicsManager::GetScene()
{
	if (instance != nullptr)
	{
		return instance->scene;
	}

	return nullptr;
}

PhysicsManager::PhysicsManager() :
	recordMemoryAllocations(true),
	defaultAllocatorCallback(),
	defaultErrorCallback(),
	physics(nullptr),
	foundation(nullptr),
	accumulator(0.0f),
	stepSize(1.0f/60.0f)
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
		else
		{
			PxSceneDesc sceneDesc(physics->getTolerancesScale());
			sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
			dispatcher = PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = dispatcher;
			sceneDesc.filterShader = PxDefaultSimulationFilterShader;

			scene = physics->createScene(sceneDesc);
		}
	}
}

PhysicsManager::~PhysicsManager()
{
	scene->release();
	dispatcher->release();
	physics->release();
	foundation->release();
}
