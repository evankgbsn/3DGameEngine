#include "CharacterController.h"

#include "../Time/TimeManager.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/Model/ModelManager.h"

CharacterController::CharacterController(float radius, float height, const glm::vec3& position)
{
	PxCapsuleControllerDesc desc;

	// --- GEOMETRY ---
	desc.radius = radius;                
	desc.height = height;                
	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	desc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);

	// --- BEHAVIOR ---
	desc.slopeLimit = cosf(PxDegToRad(45.0f)); // 45 degree slope limit
	desc.stepOffset = 0.3f;             // Can step over 30 cm obstacles
	desc.contactOffset = 0.1f;          // 10 cm skin width
	desc.material = PhysicsManager::instance->characterControllerMaterial;
	//desc.reportCallback = new MyControllerHitReport(); // Custom collision handler
	desc.behaviorCallback = nullptr;    // Use nullptr for default behavior

	// --- POSE & MANAGER ---
	desc.position = PxExtendedVec3(position.x, position.y, position.z);
	desc.userData = (void*)this;               // Pointer back to your character object

	desc.isValid();

	controller = PhysicsManager::instance->controllerManager->createController(desc);

	InitializeCapsule();
}

void CharacterController::SetHeight(float newHeight)
{
	controller->resize(newHeight);
}

void CharacterController::SetRadius(float newRadius)
{
	PxCapsuleController* capsuleController = static_cast<PxCapsuleController*>(controller);
	
	if (capsuleController != nullptr)
	{
		capsuleController->setRadius(newRadius);
	}
}

float CharacterController::GetRadius() const
{
	PxCapsuleController* capsuleController = static_cast<PxCapsuleController*>(controller);

	if (capsuleController != nullptr)
	{
		return capsuleController->getRadius();
	}

	return 0.0f;
}

float CharacterController::GetHeight() const
{
	PxCapsuleController* capsuleController = static_cast<PxCapsuleController*>(controller);

	if (capsuleController != nullptr)
	{
		return capsuleController->getHeight();
	}

	return 0.0f;
}

void CharacterController::AddDisp(const glm::vec3& newDisp)
{
	disp += newDisp;
}

void CharacterController::Move()
{
	// Remove the gravity vector entirely, just use the built-up 'disp'
	collisionFlags = controller->move(PxVec3(disp.x, disp.y, disp.z), 0.00001f, TimeManager::DeltaTime(), nullptr, nullptr);

	disp = glm::vec3(0.0f);

	capsule->SetTranslation(GetPosition());
}

PxControllerCollisionFlags CharacterController::GetCollisionFlags() const
{
	return collisionFlags;
}

glm::vec3 CharacterController::GetPosition() const
{
	PxExtendedVec3 pos = controller->getPosition();
	return { pos.x, pos.y, pos.z };
}

void CharacterController::SetPosition(const glm::vec3& newPosition)
{
	PxExtendedVec3 newPos(newPosition.x, newPosition.y, newPosition.z);
	controller->setPosition(newPos);
	capsule->SetTranslation(newPosition);
}

CharacterController::~CharacterController()
{
	GraphicsObjectManager::Delete(capsule);
	controller->release();
}

void CharacterController::SetOwner(GameObject* owner)
{
	controller->getActor()->userData = owner;
}

GameObject* CharacterController::GetOwner() const
{
	return reinterpret_cast<GameObject*>(controller->getActor()->userData);
}

void CharacterController::InitializeCapsule()
{
	if (!ModelManager::ModelLoaded("Capsule"))
	{
		ModelManager::LoadModel("Capsule", "Assets/Model/Capsule.gltf", false);
	}

	capsule = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel("Capsule"), { 1.0f, 0.5f, 0.0f, 1.0f });
	capsule->SetDrawMode(GO3D::Mode::LINE);

	// Get dimensions from PhysX
	float r = GetRadius();
	float h = GetHeight(); // Distance between centers

	// Calculate total height
	float totalHeight = h + (2.0f * r);

	// Apply to your Transform component
	// Assuming your Unit Capsule is 2.0 units tall total in Blender
	capsule->SetScale(glm::vec3(r * 2, totalHeight / 2.0f, r * 2));
}

glm::vec3 CharacterController::GetFootPosition() const
{
	PxExtendedVec3 pos = controller->getFootPosition();
	return { pos.x, pos.y, pos.z };
}
