#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include <PXConfig.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class CharacterController;

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

	static CharacterController* CreateCharacterController(const std::string& name, float radius, float height, const glm::vec3& position);

	static void DestroyCharacterController(const std::string& name);

private:

	friend class SingletonHelpers;

	friend class CharacterController;

	PhysicsManager();

	~PhysicsManager();

	void CreateCharacterControllerManager();

	void SetupVisualization();

	void CreateCharacterControllerMaterial();

	static PhysicsManager* instance;

	PxDefaultErrorCallback defaultErrorCallback;

	PxDefaultAllocator defaultAllocatorCallback;

	bool recordMemoryAllocations = true;

	PxPhysics* physics;

	PxFoundation* foundation;

	PxScene* scene;

	PxDefaultCpuDispatcher* dispatcher;

	PxControllerManager* controllerManager;

	float accumulator;

	float stepSize;

	std::unordered_map<std::string, CharacterController*> controllers;

	PxMaterial* characterControllerMaterial;
};


#endif // PHYSICSMANAGER_H