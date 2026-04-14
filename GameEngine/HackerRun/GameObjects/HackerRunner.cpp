#include "HackerRunner.h"

#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "Corruption.h"
#include "GameEngine/Renderer/Text/Text3D.h"

#include <glm/gtc/quaternion.hpp>

HackerRunner::HackerRunner() :
	GameObject("HackerRunner"),
	graphics(nullptr),
	speed(50000.0f),
	camOffset(glm::vec3(0.0f, 3.0f, -5.0f)),
	mouseSense(0.02f),
	yaw(0.0f),
	pitch(0.0f)
{
	RegisterGameObjectClassType<HackerRunner>(this);
}

HackerRunner::~HackerRunner()
{
}

void HackerRunner::Initialize()
{
	graphics = new GraphicsObjectColored(ModelManager::GetModel("HackerRunnerCube"), { 0.0f, 1.0f, 0.0f, 1.0f });
	graphics->SetRenderMode(GraphicsObject3DComponent::Mode::LINE);
	graphics->SetPosition({ 0.0f, 0.0f, 0.0f });

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, graphics->GetModel());
	body->DisableGravity();
	body->SyncPhysics();

	AddComponent(body, "RigidBody");

	cam = new CameraComponent("Player");
	cam->SetPosition(graphics->GetPosition() + camOffset);
	cam->SetTarget(graphics->GetPosition());
	cam->SetFOV(8.0f);

	AddComponent(cam, "PlayerCam");

	corruptionCollisionCallback = new std::function<void(GameObject*)>([this](GameObject* obj)
		{
			Corruption* corruption = dynamic_cast<Corruption*>(obj);

			if (corruption != nullptr)
			{
				Die();
			}
		});


	body->RegisterTriggerCallback("Corruption", corruptionCollisionCallback);

	RegisterInput();
}

void HackerRunner::Terminate()
{
	body->DeregisterTriggerCallback("HackerRunner");

	delete corruptionCollisionCallback;

	DeregisterInput();

	RemoveComponent("Graphics");
	delete graphics;
}

void HackerRunner::GameUpdate()
{
	//// NOTE: In a real engine, these should be member variables in your class/component 
	//// rather than static variables, so each object can tumble independently!
	//static glm::vec3 currentAngularVel = glm::vec3(0.0f);
	//static glm::vec3 targetAngularVel = glm::vec3(0.0f);
	//static float changeTimer = 0.0f;
	//
	//// 1. Decrease the timer by the time passed since the last frame
	//changeTimer -= TimeManager::DeltaTime();
	//
	//// 2. Pick a new random rotation axis when the timer hits zero
	//if (changeTimer <= 0.0f)
	//{
	//	// Generate random values between -1.0 and 1.0 for all three axes
	//	float rX = ((std::rand() % 200) / 100.0f) - 1.0f;
	//	float rY = ((std::rand() % 200) / 100.0f) - 1.0f;
	//	float rZ = ((std::rand() % 200) / 100.0f) - 1.0f;
	//
	//	// Normalize to get a pure directional vector, then scale by your rotation speed
	//	glm::vec3 randomAxis = glm::normalize(glm::vec3(rX, rY, rZ));
	//	targetAngularVel = randomAxis * rotationSpeed;
	//
	//	// Reset the timer to pick a new direction in the future (e.g., 2 to 4 seconds)
	//	changeTimer = 2.0f + ((std::rand() % 200) / 100.0f);
	//}
	//
	//// 3. Smoothly interpolate (Lerp) the current velocity toward the target velocity
	//// glm::mix(start, end, percentage) moves the vector a percentage of the way there.
	//// Multiplying by deltaTime ensures the smoothing is frame-rate independent.
	//float smoothingRate = 1.5f * TimeManager::DeltaTime();
	//currentAngularVel = glm::mix(currentAngularVel, targetAngularVel, smoothingRate);
	//
	//// 4. Wake the physics body up (PhysX specific safeguard to prevent sleeping)
	//// body->GetPxRigidDynamic()->wakeUp(); 
	//
	//// 5. Apply the smoothed velocity
	//body->SetAngularVelocity(currentAngularVel);

	InputManager::WhenCursorMoved(*mouseMove);

	body->SyncPhysics();
	body->Update();

	cam->SetPosition(graphics->GetPosition() + (graphics->GetUp() * camOffset.y) + (graphics->GetForward() * camOffset.z));
	cam->SetTarget(graphics->GetPosition());
	cam->SetUpVector(graphics->GetUp());

	
}

void HackerRunner::EditorUpdate()
{
}

void HackerRunner::Load()
{
	if (!ModelManager::ModelLoaded("HackerRunnerCube"))
	{
		ModelManager::LoadModel("HackerRunnerCube", "Assets/Model/Ship.gltf", false);
	}
}

void HackerRunner::Unload()
{
}

void HackerRunner::Start()
{
	GameObject::Start();

	InputManager::DisableCursor("Engine");

	cam->SetActive();
}

void HackerRunner::End()
{
	GameObject::End();

	InputManager::EnableCursor("Engine");
}

glm::vec3 HackerRunner::GetPosition() const
{
	return graphics->GetPosition();
}

void HackerRunner::SetPosition(const glm::vec3& newPos)
{
	graphics->SetPosition(newPos);
	body->SetPosition(newPos);
}

glm::mat4 HackerRunner::GetRotation() const
{
	return graphics->GetRotation();
}

void HackerRunner::SetRotation(const glm::mat4& newRot)
{
	graphics->SetRotation(newRot);
	body->SetRotation(newRot);
}

void HackerRunner::RegisterInput()
{
	keyPress = new std::function<void(int)>([this](int keyCode)
		{
			glm::vec3 axis(0.0f);

			switch (keyCode)
			{
			case KEY_W:
				//direction = graphics->GetForward();
				body->SetLinearVelocity(graphics->GetForward() * speed * TimeManager::DeltaTime());
				break;
			case KEY_A:
				axis = -graphics->GetForward();
				break;
			case KEY_S:
				//direction = -graphics->GetForward();
				body->SetLinearVelocity(-graphics->GetForward() * speed * TimeManager::DeltaTime());
				break;
			case KEY_D:
				axis = graphics->GetForward();
				break;
			default:
				break;
			}

			//body->AddTorque(axis * rollSpeed * TimeManager::DeltaTime(), RigidBodyComponent::ForceMode::VELOCITY_CHANGE);
		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, keyPress, "Movement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, keyPress, "Movement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, keyPress, "Movement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, keyPress, "Movement");

	keyRelease = new std::function<void(int)>([this](int keyCode)
		{
			glm::vec3 direction(0.0f);

			switch (keyCode)
			{
			case KEY_W:
				body->SetLinearVelocity(glm::vec3(0.0f));
				break;
			default:
				break;
			}
		});

	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_W, keyRelease, "Movement");

	mouseMove = new std::function<void(const glm::vec2&)>([this](const glm::vec2& newPos)
		{
			static glm::vec2 lastPos = newPos;

			// 1. Calculate the delta with correct parentheses and NO DeltaTime
			float xDif = -(newPos.x - lastPos.x) * mouseSense;
			float yDif = -(lastPos.y - newPos.y) * mouseSense; // Note: yDif is usually reversed 

			lastPos = newPos;

			// 2. Accumulate the rotation
			yaw += xDif;
			pitch += yDif;

			// 3. Constrain the Pitch (Prevent looking past straight up or straight down)
			// 89.0f prevents gimbal lock that can happen at exactly 90.0f
			if (pitch > 89.0f) pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;

			// 4. Construct a new rotation from the clamped values
			// Using GLM, you can create a quaternion from Euler angles (pitch, yaw, roll)
			glm::vec3 eulerRotation(glm::radians(pitch), glm::radians(yaw), 0.0f);
			glm::quat newRotation = glm::quat(eulerRotation);
			glm::mat4 newRotMat(newRotation);

			// 5. Apply to the physics body directly (Kinematic approach)
			// If your object is a character or camera, it should likely be a Kinematic Actor in PhysX.
			body->SetRotation(newRotMat); // Replace with your engine's equivalent method
		});

}

void HackerRunner::DeregisterInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "Movement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "Movement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "Movement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "Movement");

	delete keyPress;

	InputManager::DeregisterCallbackForKeyState(KEY_RELEASE, KEY_W, "Movement");

	delete keyRelease;


	delete mouseMove;
}

void HackerRunner::Die()
{
	body->SetLinearVelocity(glm::vec3(0.0f));
	body->SetAngularVelocity(glm::vec3(0.0f));
}
