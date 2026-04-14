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
	GameObject("Corruption"),
	speed(20.0f),
	range(20.0f),
	shrink(true)
{
	RegisterGameObjectClassType<Corruption>(this);
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

	AddComponent(body, "RigidBody");

	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		if (dynamic_cast<HackerRunner*>(object.second) != nullptr)
		{
			glm::mat4 rotation = object.second->GetRotation();
			glm::vec3 right = glm::normalize(glm::vec3(rotation[0]));
			glm::vec3 up = glm::normalize(glm::vec3(rotation[1]));
			glm::vec3 forward = glm::normalize(glm::vec3(rotation[2]));

			glm::vec3 offset(0.0f);
			offset += right * dis(gen);
			offset += forward * 30.0f + abs(dis(gen));

			SetPosition(object.second->GetPosition() + offset);
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

	HackerRunner* runner = nullptr;

	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		if (dynamic_cast<HackerRunner*>(object.second) != nullptr)
		{
			runner = dynamic_cast<HackerRunner*>(object.second);
			if (glm::length(object.second->GetPosition() - GetPosition()) > 100.0f)
			{
				glm::mat4 rotation = object.second->GetRotation();
				glm::vec3 right = glm::normalize(glm::vec3(rotation[0]));
				glm::vec3 up = glm::normalize(glm::vec3(rotation[1]));
				glm::vec3 forward = glm::normalize(glm::vec3(rotation[2]));

				glm::vec3 offset(0.0f);
				offset += right * dis(gen);
				offset += forward * 30.0f + abs(dis(gen));
				offset += up * abs(dis(gen));

				SetPosition(object.second->GetPosition() + offset);
			}
		}
	}

	if (runner != nullptr)
	{
		body->SetLinearVelocity(glm::normalize(runner->GetPosition() - GetPosition()) * speed);
	}

	glm::vec3 shrinkSpeed(5.0f, 4.0f, 4.50f);

	shrinkSpeed *= TimeManager::DeltaTime();

	if (shrink == false && graphics->GetScale().x >= 1.0f)
	{
		shrink = true;
	}

	if (shrink == true && graphics->GetScale().x <= 0.5f)
	{
		shrink = false;
	}

	if (shrink)
	{
		graphics->SetScale(graphics->GetScale() - shrinkSpeed);
	}
	else
	{
		graphics->SetScale(graphics->GetScale() + shrinkSpeed);
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

glm::mat4 Corruption::GetRotation() const
{
	return graphics->GetRotation();
}

void Corruption::SetRotation(const glm::mat4& newRot)
{
	graphics->SetRotation(newRot);
	body->SetRotation(newRot);
}
