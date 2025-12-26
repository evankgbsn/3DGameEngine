#include "PhysicsManager.h"

#include "../Utils/SingletonHelpers.h"
#include "../Utils/Logger.h"
#include "../Time/TimeManager.h"
#include "../Renderer/Camera/CameraManager.h"
#include "../Renderer/Camera/Camera.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Renderer/Window/Window.h"
#include "CharacterController.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Component/RigidBodyComponent.h"


//#include "cudamanager/PxCudaContextManager.h"
//#include "gpu/PxGpu.h"

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

		for (auto& controller : instance->controllers)
		{
			controller.second->Move();
		}

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

CharacterController* PhysicsManager::CreateCharacterController(const std::string& name, float radius, float height, const glm::vec3& position)
{
	if (instance != nullptr)
	{
		auto exsistingController = instance->controllers.find(name);
		if (exsistingController == instance->controllers.end())
		{
			CharacterController* newController = new CharacterController(radius, height, position);

			return instance->controllers[name] = newController;
		}
	}

	return nullptr;
}

void PhysicsManager::DestroyCharacterController(const std::string& name)
{
	if (instance != nullptr)
	{
		auto exsistingController = instance->controllers.find(name);
		if (exsistingController != instance->controllers.end())
		{
			delete exsistingController->second;
			instance->controllers.erase(exsistingController);
		}
	}
}

PxFilterFlags MyFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// Let triggers through with default trigger behavior
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// Default physical collision (bouncing/friction)
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// IMPORTANT: This line enables the onContact callback!
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
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
			dispatcher = PxDefaultCpuDispatcherCreate(4);
			sceneDesc.cpuDispatcher = dispatcher;
			sceneDesc.filterShader = MyFilterShader;

			//------------------------------------------------------------
			//  To enable physx gpu support add these libs.
			//	Currently running physx on gpu is much slower than cpu.
			//------------------------------------------------------------
			// PhysXGpu_static_64.lib
			// PhysXCommonGpu_static_64.lib
			// PhysXCudaContextManager_static_64.lib
			// PhysXNarrowphaseGpu_static_64.lib
			// PhysXSimulationControllerGpu_static_64.lib
			// PhysXBroadphaseGpu_static_64.lib
			// PhysXArticulationGpu_static_64.lib
			// PhysXSolverGpu_static_64.lib
			// cuda.lib
			// cudart.lib
			//------------------------------------------------------------
			// And uncomment this
			//------------------------------------------------------------
			//// Assuming m_PxFoundation is your PxFoundation*
			//PxCudaContextManagerDesc cudaDesc;
			//// Configure cudaDesc fields (e.g., interopMode)
			//
			//// Create the PxCudaContextManager
			//cudaContextManager = PxCreateCudaContextManager(*foundation, cudaDesc);
			//
			//// Check if the context is valid (i.e., a compatible GPU was found)
			//if (cudaContextManager && !cudaContextManager->contextIsValid())
			//{
			//	// Handle error: context creation failed (e.g., no CUDA-capable GPU)
			//	cudaContextManager->release();
			//	cudaContextManager = nullptr;
			//}
			//
			//// Enable CUDA GPU support
			//if (cudaContextManager)
			//{
			//	// Assign the context manager
			//	//sceneDesc.cudaContextManager = cudaContextManager;
			//	//sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
			//	//sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
			//}
			//------------------------------------------------------------

			MyContactCallback* myCallback = new MyContactCallback();
			sceneDesc.simulationEventCallback = myCallback;

			scene = physics->createScene(sceneDesc);

			SetupVisualization();
			CreateCharacterControllerManager();
			CreateCharacterControllerMaterial();
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

void PhysicsManager::CreateCharacterControllerManager()
{
	controllerManager = PxCreateControllerManager(*scene);
}

void PhysicsManager::SetupVisualization()
{
	scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	scene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_POINT, 1.0f);
	scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
}

void PhysicsManager::CreateCharacterControllerMaterial()
{
	// Create a material with your desired properties
	// Parameters are: staticFriction, dynamicFriction, restitution
	PxReal staticFriction = 0.5f;
	PxReal dynamicFriction = 0.5f;
	PxReal restitution = 0.0f; // 0.0 means no bounce

	characterControllerMaterial = physics->createMaterial(staticFriction, dynamicFriction, restitution);
}

void MyContactCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	for (PxU32 i = 0; i < nbPairs; i++) {
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			// Objects just started touching
			PxActor* actorA = pairHeader.actors[0];
			PxActor* actorB = pairHeader.actors[1];

			GameObject* goA = static_cast<GameObject*>(actorA->userData);
			RigidBodyComponent* rbcA = dynamic_cast<RigidBodyComponent*>(goA->GetComponent("RigidBody"));

			GameObject* goB = static_cast<GameObject*>(actorB->userData);
			RigidBodyComponent* rbcB = dynamic_cast<RigidBodyComponent*>(goB->GetComponent("RigidBody"));

			rbcA->RegisterContact(rbcB);
			rbcB->RegisterContact(rbcA);

			Logger::Log(goA->GetName() + " : " + goB->GetName());
		}
	}
}

void MyContactCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void MyContactCallback::onWake(PxActor** actors, PxU32 count)
{
}

void MyContactCallback::onSleep(PxActor** actors, PxU32 count)
{
}

void MyContactCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
}

void MyContactCallback::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}
