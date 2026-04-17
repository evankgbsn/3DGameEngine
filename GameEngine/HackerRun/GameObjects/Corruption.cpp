#include "Corruption.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Scene/Scene.h"
#include "HackerRunner.h"
#include "Spawner.h"
#include "MemoryBlock.h"
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
	shrink(true),
	respawnDistance(400.0f)
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

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, graphics->GetModel());
	body->DisableGravity();
	body->SetIsTrigger(true);
	body->SetPosition(graphics->GetPosition());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");

	SetSpawnPosition();
}

void Corruption::Terminate()
{
	RemoveComponent("Graphics");
	delete graphics;
}

void Corruption::GameUpdate()
{
	Respawn();
	UpdateVelocity();
	Animate();
	
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

void Corruption::SetSpawnPosition()
{
	Spawner* spawner = FindSpawner();
	HackerRunner* player = FindPlayer();

	if (spawner == nullptr || player == nullptr)
	{
		return;
	}

	const std::vector<std::vector<MemoryBlock*>>& blocks = spawner->GetBlocks();

	glm::vec3 closestBlockPos = glm::vec3(1.0f) * FLT_MAX;

	glm::vec3 playerForward = glm::normalize(glm::vec3(player->GetRotation()[2]));

	glm::vec3 playerProjectForward = player->GetPosition() + playerForward * 200.0f;

	for (const auto& row : blocks)
	{
		for (const auto& block : row)
		{
			if (glm::length(playerProjectForward - block->GetPosition()) < glm::length(closestBlockPos - playerProjectForward))
			{
				closestBlockPos = block->GetPosition();
			}
		}
	}

	float spaceBetweenBlocks = spawner->GetSpaceBetweenBlocks();
	float halfSpace = spaceBetweenBlocks / 2.0f;

	// 8. Determine the primary viewing axis using the 'forward' vector
	bool lookingAlongX = std::abs(playerForward.x) > std::abs(playerForward.z);

	if (lookingAlongX)
	{
		// Snap Z to nearest gap, project line along the X-axis
		float nearestGapZ = std::round((playerProjectForward.z - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
		float dirSign = (playerForward.x > 0.0f) ? 1.0f : -1.0f; // Preserve whether they are looking Left or Right

		SetPosition(glm::vec3(playerProjectForward.x, player->GetPosition().y, nearestGapZ));
	}
	else
	{
		// Snap X to nearest gap, project line along the Z-axis
		float nearestGapX = std::round((playerProjectForward.x - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
		float dirSign = (playerForward.z > 0.0f) ? 1.0f : -1.0f; // Preserve whether they are looking Forward or Backward

		SetPosition(glm::vec3(nearestGapX, player->GetPosition().y, playerProjectForward.z));
	}
}

void Corruption::Respawn()
{
	Spawner* spawner = FindSpawner();
	HackerRunner* player = FindPlayer();

	if (spawner == nullptr || player == nullptr)
	{
		return;
	}

	if (glm::length(player->GetPosition() - GetPosition()) < respawnDistance)
	{
		return;
	}

	const std::vector<std::vector<MemoryBlock*>>& blocks = spawner->GetBlocks();

	glm::vec3 closestBlockPos = glm::vec3(1.0f) * FLT_MAX;

	glm::vec3 playerForward = glm::normalize(glm::vec3(player->GetRotation()[2]));

	glm::vec3 playerProjectForward = player->GetPosition() + playerForward * player->GetSpeed() * 50.0f;

	for (const auto& row : blocks)
	{
		for (const auto& block : row)
		{
			if (glm::length(playerProjectForward - block->GetPosition()) < glm::length(closestBlockPos - playerProjectForward))
			{
				closestBlockPos = block->GetPosition();
			}
		}
	}

	float spaceBetweenBlocks = spawner->GetSpaceBetweenBlocks();
	float halfSpace = spaceBetweenBlocks / 2.0f;

	// 8. Determine the primary viewing axis using the 'forward' vector
	bool lookingAlongX = std::abs(playerForward.x) > std::abs(playerForward.z);

	if (lookingAlongX)
	{
		// Snap Z to nearest gap, project line along the X-axis
		float nearestGapZ = std::round((playerProjectForward.z - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
		float dirSign = (playerForward.x > 0.0f) ? 1.0f : -1.0f; // Preserve whether they are looking Left or Right

		SetPosition(glm::vec3(playerProjectForward.x, player->GetPosition().y, nearestGapZ));
	}
	else
	{
		// Snap X to nearest gap, project line along the Z-axis
		float nearestGapX = std::round((playerProjectForward.x - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
		float dirSign = (playerForward.z > 0.0f) ? 1.0f : -1.0f; // Preserve whether they are looking Forward or Backward

		SetPosition(glm::vec3(nearestGapX, player->GetPosition().y, playerProjectForward.z));
	}
}

void Corruption::Animate()
{
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
}

HackerRunner* Corruption::FindPlayer() const
{
	HackerRunner* runner = nullptr;

	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		if (dynamic_cast<HackerRunner*>(object.second) != nullptr)
		{
			runner = dynamic_cast<HackerRunner*>(object.second);
		}
	}

	return runner;
}

Spawner* Corruption::FindSpawner() const
{
	Spawner* spawner = nullptr;

	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		if (dynamic_cast<Spawner*>(object.second) != nullptr)
		{
			spawner = dynamic_cast<Spawner*>(object.second);
		}
	}

	return spawner;
}

void Corruption::UpdateVelocity()
{
	HackerRunner* runner = FindPlayer();
	if (runner != nullptr)
	{
		body->SetLinearVelocity(glm::normalize(runner->GetPosition() - GetPosition()) * speed);
	}
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
