#include "SpotLightObject.h"

#include "GameEngine/GameObject/Component/SpotLightComponent.h"
#include "GameEngine/Editor/Editor.h"

SpotLightObject::SpotLightObject() :
	GameObject("SpotLightObject"),
	light(nullptr),
	onEditorEnable(nullptr),
	onEditorDisable(nullptr)
{
	RegisterGameObjectClassType<SpotLightObject>(this);
}

SpotLightObject::~SpotLightObject()
{

}

void SpotLightObject::Initialize()
{
	light = new SpotLightComponent({ 1.50f, 3.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, -1.0f, 0.0f});

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

void SpotLightObject::Terminate()
{
	TerminateEditorCallbacks();

	RemoveComponent("Light");

	delete light;
}

void SpotLightObject::GameUpdate()
{
}

void SpotLightObject::EditorUpdate()
{
}

void SpotLightObject::Load()
{
}

void SpotLightObject::Unload()
{
}

void SpotLightObject::Start()
{
	light->SetColliderVisibility(false);
}

void SpotLightObject::SetPosition(const glm::vec3& pos)
{
	light->SetPosition(pos);
}

glm::vec3 SpotLightObject::GetPosition() const
{
	return light->GetPosition();
}

void SpotLightObject::SetRotation(const glm::mat4& rot)
{
}

glm::mat4 SpotLightObject::GetRotation() const
{
	return glm::mat4(1.0f);
}

glm::mat4 SpotLightObject::GetTransform() const
{
	return glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(light->GetPosition(), 1.0f));
}

bool SpotLightObject::Hovered() const
{
	return light->Hovered();
}

void SpotLightObject::Deserialize()
{
	GameObject::Deserialize();
}

void SpotLightObject::InitializeEditorCallbacks()
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

void SpotLightObject::TerminateEditorCallbacks()
{
	Editor::DeregisterOnEditorDisable(onEditorDisable);
	Editor::DeregisterOnEditorEnable(onEditorEnable);

	delete onEditorDisable;
	delete onEditorEnable;
}
