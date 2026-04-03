#include "PointLightObject.h"

#include "GameEngine/GameObject/Component/PointLightComponent.h"
#include "GameEngine/Editor/Editor.h"

PointLightObject::PointLightObject() :
	GameObject("PointLightObject"),
	light(nullptr),
	onEditorEnable(nullptr),
	onEditorDisable(nullptr)
{
	RegisterGameObjectClassType<PointLightObject>(this);
}

PointLightObject::~PointLightObject()
{
	
}

void PointLightObject::Initialize()
{
	light = new PointLightComponent({ 1.50f, 3.0f, 3.0f }, { 0.0f, 0.0f, 0.0f });
	
	if (Editor::IsEnabled())
	{
		light->SetColliderVisibility(true);
	}
	else
	{
		light->SetColliderVisibility(false);
	}

	AddComponent(light, "Light");

	InitializeEditorCallbacks();
}

void PointLightObject::Terminate()
{
	TerminateEditorCallbacks();

	RemoveComponent("Light");

	delete light;
}

void PointLightObject::GameUpdate()
{
}

void PointLightObject::EditorUpdate()
{
}

void PointLightObject::Load()
{
}

void PointLightObject::Unload()
{
}

void PointLightObject::Start()
{
	light->SetColliderVisibility(false);
}

void PointLightObject::SetPosition(const glm::vec3& pos)
{
	light->SetPosition(pos);
}

glm::vec3 PointLightObject::GetPosition() const
{
	return light->GetPosition();
}

void PointLightObject::SetRotation(const glm::mat4& rot)
{
}

glm::mat4 PointLightObject::GetRotation() const
{
	return glm::mat4(1.0f);
}

glm::mat4 PointLightObject::GetTransform() const
{
	return glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(light->GetPosition(), 1.0f));
}

bool PointLightObject::Hovered() const
{
	return light->Hovered();
}

void PointLightObject::Deserialize()
{
	GameObject::Deserialize();
}

void PointLightObject::InitializeEditorCallbacks()
{
	onEditorEnable = new std::function<void()>([this]()
		{
			light->SetColliderVisibility(true);
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);

	onEditorDisable = new std::function<void()>([this]()
		{
			light->SetColliderVisibility(false);
		});

	Editor::RegisterOnEditorDisable(onEditorDisable);
}

void PointLightObject::TerminateEditorCallbacks()
{
	Editor::DeregisterOnEditorDisable(onEditorDisable);
	Editor::DeregisterOnEditorEnable(onEditorEnable);

	delete onEditorDisable;
	delete onEditorEnable;
}
