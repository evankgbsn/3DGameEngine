#include "CharacterController.h"

#include "../Time/TimeManager.h"

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
}

void CharacterController::SetHeight(float newHeight)
{
	controller->resize(newHeight);
}

void CharacterController::SetRadius(float newRadius)
{
	PxCapsuleController* capsuleController = dynamic_cast<PxCapsuleController*>(controller);
	
	if (capsuleController != nullptr)
	{
		capsuleController->setRadius(newRadius);
	}
}

float CharacterController::GetRadius() const
{
	PxCapsuleController* capsuleController = dynamic_cast<PxCapsuleController*>(controller);

	if (capsuleController != nullptr)
	{
		return capsuleController->getRadius();
	}

	return 0.0f;
}

float CharacterController::GetHeight() const
{
	PxCapsuleController* capsuleController = dynamic_cast<PxCapsuleController*>(controller);

	if (capsuleController != nullptr)
	{
		return capsuleController->getHeight();
	}

	return 0.0f;
}

void CharacterController::SetDisp(const glm::vec3& newDisp)
{
	disp = newDisp;
}

void CharacterController::Move()
{
	glm::vec3 gravity = glm::vec3(0.0f, 1.0f, 0.0f) * -0.098f;

	controller->move(PxVec3(disp.x + gravity.x, disp.y + gravity.y, disp.z + gravity.z), 0.001f, TimeManager::DeltaTime(), nullptr, nullptr);

	disp = glm::vec3(0.0f);
}

glm::vec3 CharacterController::GetPosition() const
{
	PxExtendedVec3 pos = controller->getPosition();
	return { pos.x, pos.y, pos.z };
}

CharacterController::~CharacterController()
{
	controller->release();
}
