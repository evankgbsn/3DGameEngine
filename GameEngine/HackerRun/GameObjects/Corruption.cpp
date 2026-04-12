#include "Corruption.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Scene/Scene.h"
#include "HackerRunner.h"
#include "GameEngine/Time/TimeManager.h"

#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(-10.0f, 10.0f);

Corruption::Corruption() :
	GameObject("Corruption")
{
	
}

Corruption::~Corruption()
{
}

void Corruption::Initialize()
{
	graphics = new GraphicsObjectColored(ModelManager::GetModel("Corruption"), { 1.0f, 0.0f, 0.0f, 1.0f });
	graphics->SetRenderMode(GraphicsObject3DComponent::Mode::LINE);
	graphics->SetPosition({ 0.0f, 0.0f, 10.0f });

	AddComponent(graphics, "Graphics");
}

void Corruption::Terminate()
{
	RemoveComponent("Graphics");
	delete graphics;
}

void Corruption::GameUpdate()
{
	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		if (dynamic_cast<HackerRunner*>(object.second) != nullptr)
		{
			if (glm::length(object.second->GetPosition() - GetPosition()) > 40.0f)
			{
				SetPosition(object.second->GetPosition() + glm::vec3(dis(gen), 0.0f, 15.0f + abs(dis(gen))));
			}
		}
	}
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
	graphics->SetPosition(newPos);
}
