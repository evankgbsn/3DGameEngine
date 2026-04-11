#include "ShippingContainer.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLitInstanced.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Scene/Scene.h"
#include "GameEngine/GameObject/Component/EmptyComponent.h"

GraphicsObjectTexturedLitInstanced* ShippingContainer::graphics = nullptr;

ShippingContainer::ShippingContainer() :
	GameObject("ShippingContainer"),
	body(nullptr)
{
	RegisterGameObjectClassType<ShippingContainer>(this);
}

ShippingContainer::~ShippingContainer()
{
}

void ShippingContainer::Initialize()
{
	empty = new EmptyComponent();

	AddComponent(empty, "Empty");

	if (graphics == nullptr)
	{
		graphics = new GraphicsObjectTexturedLitInstanced(
			"ShippingContainer",
			"ShippingContainerRed",
			"ShippingContainerSpec",
			"ShippingContainerNormal", 1);

		graphics->SetShine(4.0f);

		AddComponent(graphics, "Graphics");

		instanceID = 0;
	}
	else
	{
		unsigned int i = 0;
		for (const auto& obj : GetOwningScene()->GetGameObjects())
		{
			if (dynamic_cast<ShippingContainer*>(obj.second) != nullptr && obj.second != this)
			{
				i++;
			}
		}

		if (graphics->GetInstanceCount() <= i)
		{
			instanceID = graphics->AddInstance();
		}
	}
	

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");

	empty->SetInt("InstanceID", instanceID);
}

void ShippingContainer::Terminate()
{
	RemoveComponent("Empty");
	delete empty;

	RemoveComponent("RigidBody");
	delete body;

	if (graphics->GetInstanceCount() == 1)
	{
		RemoveComponent("Graphics");
		delete graphics;
		graphics = nullptr;
	}
	else
	{
		graphics->RemoveInstanceByID(instanceID);
	}
}

void ShippingContainer::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void ShippingContainer::EditorUpdate()
{
}

void ShippingContainer::Load()
{
	if (!ModelManager::ModelLoaded("ShippingContainer"))
	{
		ModelManager::LoadModel("ShippingContainer", "Assets/Model/ShippingContainer.gltf", false);
	}

	if (!TextureManager::TextureLoaded("ShippingContainerRed"))
	{
		TextureManager::LoadTexture("Assets/Texture/RedDiffuse.png", "ShippingContainerRed");
	}

	if (!TextureManager::TextureLoaded("ShippingContainerSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/RoughnessMap.png", "ShippingContainerSpec");
	}

	if (!TextureManager::TextureLoaded("ShippingContainerNormal"))
	{
		TextureManager::LoadTexture("Assets/Texture/ShippingContainerNormal.png", "ShippingContainerNormal");
	}
}

void ShippingContainer::Unload()
{
}

void ShippingContainer::SetPosition(const glm::vec3& pos)
{
	graphics->SetTranslation(pos, instanceID);
	body->SetPosition(graphics->GetTranslation(instanceID));
}

glm::vec3 ShippingContainer::GetPosition() const
{
	return graphics->GetTranslation(instanceID);
}

void ShippingContainer::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot, instanceID);
	body->SetRotation(graphics->GetRotation(instanceID));
}

glm::mat4 ShippingContainer::GetRotation() const
{
	return graphics->GetRotation(instanceID);
}

glm::mat4 ShippingContainer::GetTransform() const
{
	return graphics->GetTransform(instanceID);
}

bool ShippingContainer::Hovered() const
{
	return body->Hovered();
}

void ShippingContainer::Deserialize()
{
	GameObject::Deserialize();

	if (dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("Graphics")) != nullptr)
	{
		if (graphics != nullptr)
		{
			delete graphics;
		}

		graphics = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("Graphics"));
	}

	instanceID = static_cast<EmptyComponent*>(GetComponent("Empty"))->GetInt("InstanceID");

	body->SetOwner(this);
	body->SyncPhysics();
}
