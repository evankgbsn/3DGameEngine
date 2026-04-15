#include "MemoryBlock.h"

#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Text/Text3D.h"
#include "GameEngine/Time/TimeManager.h"

#include <random>

extern std::uniform_real_distribution<float> dis;
extern std::mt19937 gen;

MemoryBlock::MemoryBlock() :
	GameObject("MemoryBlock"),
	binary(nullptr)
{
	RegisterGameObjectClassType<MemoryBlock>(this);
}

MemoryBlock::~MemoryBlock()
{
}

glm::vec3 MemoryBlock::GetPosition() const
{
	return graphics->GetPosition();
}

void MemoryBlock::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

glm::mat4 MemoryBlock::GetRotation() const
{
	return graphics->GetRotation();
}

void MemoryBlock::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::mat4 MemoryBlock::GetTransform() const
{
	return graphics->GetTransform();
}

void MemoryBlock::Initialize()
{
	graphics = new GraphicsObjectColored(ModelManager::GetModel("MemoryBlock"), glm::vec4(0.0f, 0.2f, 1.0f, 1.0f));
	graphics->SetRenderMode(GraphicsObject3DComponent::Mode::LINE);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();
	body->Update();

	AddComponent(body, "RigidBody");
}

void MemoryBlock::Terminate()
{
}

void MemoryBlock::GameUpdate()
{ 
	//if (TimeManager::SecondsSinceStart() - lastUpdateTime > 0.050f)
	//{
	//	lastUpdateTime = TimeManager::SecondsSinceStart();
	//
	//	if (binary != nullptr)
	//	{
	//		std::string s = binary->GetString();
	//		std::string newS = s;
	//
	//		unsigned int i = 0;
	//		for (i; i < s.size() - 1; ++i)
	//		{
	//			newS[i + 1] = s[i];
	//		}
	//
	//		newS[0] = ((int)(TimeManager::DeltaTime() * 1000.0f) % 2 == 0) ? '0' : '1';
	//
	//		binary->SetText3D(newS);
	//	}
	//}
}

void MemoryBlock::EditorUpdate()
{
}

void MemoryBlock::Load()
{
	if (!ModelManager::ModelLoaded("MemoryBlock"))
	{
		ModelManager::LoadModel("MemoryBlock", "Assets/Model/MemoryBlock.gltf", false);
	}
}

void MemoryBlock::Unload()
{
}

void MemoryBlock::Start()
{
	GameObject::Start();

	CreateBinaryEffect();
}

void MemoryBlock::End()
{
	delete binary;
}

void MemoryBlock::CreateBinaryEffect()
{
	//binaryEffect = std::vector<std::vector<Text3D*>>();
	//binaryEffect.resize(4);
	//
	//glm::vec3 sideOffsets[4] =
	//{
	//	glm::vec3(37.25f, 0.0f, 0.0f),
	//	glm::vec3(-37.25f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 0.0f, -37.25f),
	//	glm::vec3(0.0f, 0.0f, 37.25f)
	//};
	//
	//float rotations[4]
	//{
	//	90.0f,
	//	270.0f,
	//	180.0f,
	//	0.0f
	//};
	//
	//unsigned int i = 0;
	//for (auto& text : binaryEffect)
	//{
	//	float j = 0.0f;
	//	text.resize(10);
	//	for (Text3D* ptr : text)
	//	{
	//		ptr = new Text3D("010101010101010101010101", "arial", { 1.0f, 1.0f, 1.0f, 1.0f }, graphics->GetPosition() + sideOffsets[i] + glm::vec3(0.0f, 1.0f, 0.0f) * j++ * 10.0f, {.010f, .010f});
	//		ptr->SetRotation(rotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
	//	}
	//
	//	i++;
	//}
}

void MemoryBlock::UpdateBinaryEffects()
{
}
