#include "HackerRunner.h"

#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "Corruption.h"

HackerRunner::HackerRunner() :
	GameObject("HackerRunner"),
	graphics(nullptr),
	speed(2.0f),
	camOffset(glm::vec3(0.0f, 3.0f, -8.0f)),
	strafeSpeedIncreaseInterval(4.0f),
	strafeSpeed(10.0f)
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

	cam = new CameraComponent("Player");
	cam->SetPosition(graphics->GetPosition() + camOffset);
	cam->SetTarget(graphics->GetPosition());

	AddComponent(cam, "PlayerCam");

	corruptionCollisionCallback = new std::function<void(GameObject*)>([this](GameObject* obj)
		{
			Corruption* corruption = dynamic_cast<Corruption*>(obj);

			if (corruption != nullptr)
			{
				Die();
			}
		});


	body->RegisterContactCallback("HackerRunner", corruptionCollisionCallback);

	RegisterInput();
}

void HackerRunner::Terminate()
{
	body->DeregisterContactCallback("HackerRunner");

	delete corruptionCollisionCallback;

	DeregisterInput();

	RemoveComponent("Graphics");
	delete graphics;
}

void HackerRunner::GameUpdate()
{
	body->Update();
	body->SyncPhysics();

	cam->SetPosition(graphics->GetPosition() + camOffset);
	cam->SetTarget(graphics->GetPosition());

	body->AddForce(graphics->GetForward() * speed * TimeManager::DeltaTime(), RigidBodyComponent::ForceMode::VELOCITY_CHANGE);

	strafeSpeed += strafeSpeedIncreaseInterval * TimeManager::DeltaTime();
}

void HackerRunner::EditorUpdate()
{
}

void HackerRunner::Load()
{
	if (!ModelManager::ModelLoaded("HackerRunnerCube"))
	{
		ModelManager::LoadModel("HackerRunnerCube", "Assets/Model/Cube.gltf", false);
	}
}

void HackerRunner::Unload()
{
}

void HackerRunner::Start()
{
	GameObject::Start();

	cam->SetActive();
}

glm::vec3 HackerRunner::GetPosition() const
{
	return graphics->GetPosition();
}

void HackerRunner::SetPosition(const glm::vec3& newPos)
{
	graphics->SetPosition(newPos);
}

void HackerRunner::RegisterInput()
{
	keyPress = new std::function<void(int)>([this](int keyCode)
		{
			glm::vec3 direction(0.0f);

			switch (keyCode)
			{
			case KEY_W:
				//direction = graphics->GetForward();
				break;
			case KEY_A:
				direction = graphics->GetRight();
				break;
			case KEY_S:
				//direction = -graphics->GetForward();
				break;
			case KEY_D:
				direction = -graphics->GetRight();
				break;
			default:
				break;
			}

			body->AddForce(direction * strafeSpeed * TimeManager::DeltaTime(), RigidBodyComponent::ForceMode::VELOCITY_CHANGE);
		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, keyPress, "Movement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, keyPress, "Movement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, keyPress, "Movement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, keyPress, "Movement");

}

void HackerRunner::DeregisterInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "Movement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "Movement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "Movement");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "Movement");
}

void HackerRunner::Die()
{
	body->SetLinearVelocity(glm::vec3(0.0f));
	body->SetAngularVelocity(glm::vec3(0.0f));
}
