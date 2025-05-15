#include "PhysicsManager.h"

#include "../Utils/SingletonHelpers.h"
#include "../Utils/Logger.h"
#include "../Time/TimeManager.h"
#include "../Renderer/Camera/CameraManager.h"
#include "../Renderer/Camera/Camera.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Renderer/Window/Window.h"

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

bool PhysicsManager::Raycast(const glm::vec3& o, const glm::vec3& direction, float dist, PxRaycastBuffer& outHit)
{
	bool status = false;

	if (instance != nullptr)
	{
		PxVec3 origin(o.x, o.y, o.z);                 // [in] Ray origin
		PxVec3 unitDir(direction.x, direction.y, direction.z);                // [in] Normalized ray direction
		PxReal maxDistance(dist);            // [in] Raycast max distance

		// Raycast against all static & dynamic objects (no filtering)
		// The main result from this call is the closest hit, stored in the 'hit.block' structure
		status = instance->scene->raycast(origin, unitDir, maxDistance, outHit);
	}

	return status;
}

bool PhysicsManager::RaycastFromCursor(float maxDistance, PxRaycastBuffer& outHit)
{
	Camera& cam = CameraManager::GetActiveCamera();

	// Screen space to world space for object picking.
	Window* window = WindowManager::GetWindow("Engine");
	glm::vec2 cursorPos = window->GetCursorPosition();
	glm::mat4 invPersp = glm::inverse(cam.GetProjection());
	glm::mat4 invView = glm::inverse(cam.GetView());
	glm::mat4 screenToNDC(
		glm::vec4((float)window->GetWidth() / 2.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0, -(float)window->GetHeight() / 2.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4((float)window->GetWidth() / 2.0f, (float)window->GetHeight() / 2.0f, 0.5f, 1.0f)
	);
	screenToNDC = glm::inverse(screenToNDC);

	glm::vec4 x = glm::vec4(cursorPos.x, cursorPos.y, 1.0f, 1.0f);
	x = screenToNDC * x;
	x = invPersp * x;
	x = invView * x;
	x /= x.w;

	glm::vec3 direction = glm::normalize(glm::vec3(x) - cam.GetPosition());


	return Raycast(cam.GetPosition(), direction, maxDistance, outHit);
}

PhysicsManager::PhysicsManager() :
	recordMemoryAllocations(true),
	defaultAllocatorCallback(),
	defaultErrorCallback(),
	physics(nullptr),
	foundation(nullptr),
	accumulator(0.0f),
	stepSize(1.0f/240.0f)
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
