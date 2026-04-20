#include "SkySphere.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTextured.h"

#include <glm/gtc/matrix_transform.hpp>

SkySphere::SkySphere() :
	GameObject("SkySphere"),
	sky(nullptr)
{
	RegisterGameObjectClassType<SkySphere>(this);
}

SkySphere::~SkySphere()
{
}

void SkySphere::Initialize()
{
	sky = new GraphicsObjectTextured(ModelManager::GetModel("Sky"), TextureManager::GetTexture("Sky"));
	sky->SetPosition({ 0.0f, 0.0f, 0.0f });

	AddComponent(sky, "Graphics");
}

void SkySphere::Terminate()
{
	RemoveComponent("Graphics");
	delete sky;
}

void SkySphere::GameUpdate()
{
}

void SkySphere::EditorUpdate()
{
}

void SkySphere::Load()
{
	if (!TextureManager::TextureLoaded("Sky"))
	{
		TextureManager::LoadTexture("Assets/Texture/Sky.png", "Sky");
	}

	if (!ModelManager::ModelLoaded("Sky"))
	{
		ModelManager::LoadModel("Sky", "Assets/Model/SkySphere.gltf", false);
	}
}

void SkySphere::Unload()
{
}

void SkySphere::Start()
{
	sky->SetPosition(glm::vec3(0.0f, -200.0f, 0.0f));
	sky->SetRotation(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}

bool SkySphere::Hovered() const
{
	return false;
}

glm::vec3 SkySphere::GetPosition() const
{
	return glm::vec3();
}

void SkySphere::SetPosition(const glm::vec3& newPos)
{
}

glm::mat4 SkySphere::GetRotation() const
{
	return glm::mat4();
}

void SkySphere::SetRotation(const glm::mat4& newRot)
{
}

glm::mat4 SkySphere::GetTransform() const
{
	return glm::mat4();
}