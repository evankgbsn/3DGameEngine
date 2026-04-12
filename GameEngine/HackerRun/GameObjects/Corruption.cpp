#include "Corruption.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Scene/Scene.h"
#include "HackerRunner.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(-20.0f, 20.0f);

Corruption::Corruption() :
	GameObject("Corruption")
{
	
}

Corruption::~Corruption()
{
	delete body;
	delete graphics;
}

void Corruption::Initialize()
{
	graphics = new GraphicsObjectColored(ModelManager::GetModel("Corruption"), { 1.0f, 0.0f, 0.0f, 1.0f });
	graphics->SetRenderMode(GraphicsObject3DComponent::Mode::LINE);

	body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, graphics->GetModel());
	body->DisableGravity();
	body->SetIsTrigger(true);
	body->SetPosition(graphics->GetPosition());
	body->SyncPhysics();

	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		if (dynamic_cast<HackerRunner*>(object.second) != nullptr)
		{
			SetPosition(object.second->GetPosition() + glm::vec3(dis(gen), 0.0f, 30.0f + abs(dis(gen))));
		}
	}

	AddComponent(graphics, "Graphics");
}

void Corruption::Terminate()
{
	RemoveComponent("Graphics");
	delete graphics;
}

void Corruption::GameUpdate()
{
	range += 1.0f * TimeManager::DeltaTime();

	dis = std::uniform_real_distribution<float>(-range, range);

	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		if (dynamic_cast<HackerRunner*>(object.second) != nullptr)
		{
			if (glm::length(object.second->GetPosition() - GetPosition()) > 40.0f)
			{
				SetPosition(object.second->GetPosition() + glm::vec3(dis(gen), 0.0f, 30.0f + abs(dis(gen))));
			}
		}
	}

	body->SyncPhysics();
	body->Update();
}

void Corruption::EditorUpdate()
{
}

void Corruption::Load()
{
	if (!ModelManager::ModelLoaded("Corruption"))
	{
		ModelManager::LoadModel("Corruption", "Assets/Model/Corruption.gltf", false);
	}
}

void Corruption::Unload()
{
}

void Corruption::Start()
{
}

glm::vec3 Corruption::GetPosition() const
{
	return graphics->GetPosition();
}

void Corruption::SetPosition(const glm::vec3& newPos)
{
	body->SetPosition(newPos);
	graphics->SetPosition(newPos);
}
