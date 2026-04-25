#include "TestSoundObj.h"

#include "GameEngine/Managers.h"

#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Audio/AudioManager.h"
#include "GameEngine/Audio/Source.h"

TestSoundObj::TestSoundObj() :
	GameObject("TestSoundObj")
{
	RegisterGameObjectClassType<TestSoundObj>(this);
}

TestSoundObj::~TestSoundObj()
{
}

void TestSoundObj::Initialize()
{
	graphics = new GraphicsObjectColored(ModelManager::GetModel("Sphere"), { 1.0f, 0.0f, 0.0f, 1.0f });
	graphics->SetPosition({ 0.0f, 3.0f, 0.0f });

	AddComponent(graphics, "Graphics");

	audioSource = AudioManager::CreateSource("Beep", graphics->GetPosition(), graphics->GetRotation());
	audioSource->AddSound("Beep");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();
	body->Update();

	AddComponent(body, "RigidBody");
}

void TestSoundObj::Terminate()
{
	RemoveComponent("Graphics");
	RemoveComponent("RigidBody");

	delete graphics;
	delete body;
}

void TestSoundObj::GameUpdate()
{
	static float time = TimeManager::SecondsSinceStart();

	if (TimeManager::SecondsSinceStart() - time > 2.0f)
	{
		audioSource->Play("Beep");
		time = TimeManager::SecondsSinceStart();
	}
}

void TestSoundObj::EditorUpdate()
{
}

void TestSoundObj::Load()
{
	if (!AudioManager::SoundLoaded("Beep"))
	{
		AudioManager::LoadSound("Beep", "Assets/Sound/Beep.wav");
	}
}

void TestSoundObj::Unload()
{
}

bool TestSoundObj::Hovered() const
{
	return body->Hovered();
}

void TestSoundObj::Start()
{
	
}

glm::vec3 TestSoundObj::GetPosition() const
{
	return graphics->GetPosition();
}

void TestSoundObj::SetPosition(const glm::vec3& newPos)
{
	body->SetPosition(newPos);
	graphics->SetPosition(newPos);
	audioSource->SetPosition(newPos);
}

glm::mat4 TestSoundObj::GetRotation() const
{
	return graphics->GetRotation();
}

void TestSoundObj::SetRotation(const glm::mat4& newRot)
{
	body->SetRotation(newRot);
	graphics->SetRotation(newRot);
	audioSource->SetRotation(newRot);
}

glm::mat4 TestSoundObj::GetTransform() const
{
	return graphics->GetTransform();
}