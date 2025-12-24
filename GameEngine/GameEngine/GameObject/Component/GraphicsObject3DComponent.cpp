#include "GraphicsObject3DComponent.h"

#include "../../Renderer/GraphicsObjects/GO3D.h"
#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/Model/Model.h"

GraphicsObject3DComponent::GraphicsObject3DComponent() :
	graphics(nullptr)
{
}

GraphicsObject3DComponent::~GraphicsObject3DComponent()
{
	GraphicsObjectManager::Delete(graphics);
}

void GraphicsObject3DComponent::SetRenderGraphics(bool val)
{
	graphics->SetRenderGraphics(val);
}

void GraphicsObject3DComponent::SetRenderShadow(bool val)
{
	graphics->SetRenderShadow(val);
}

void GraphicsObject3DComponent::SetRenderReflection(bool val)
{
	graphics->SetRenderReflection(val);
}

bool GraphicsObject3DComponent::RenderGraphics() const
{
	return graphics->RenderGraphics();
}

bool GraphicsObject3DComponent::RenderShadow() const
{
	return graphics->RenderShadow();
}

bool GraphicsObject3DComponent::RenderReflection() const
{
	return graphics->RenderReflection();
}

void GraphicsObject3DComponent::Serialize()
{
	savedMat4s["Transform"] = GetTransform();
}

void GraphicsObject3DComponent::Deserialize()
{
	SetTransform(savedMat4s["Transform"]);
}

glm::vec3 GraphicsObject3DComponent::GetPosition() const
{
	return graphics->GetTranslation();
}

glm::mat4 GraphicsObject3DComponent::GetRotation() const
{
	return graphics->GetRotation();
}

glm::vec3 GraphicsObject3DComponent::GetScale() const
{
	return graphics->GetScale();
}

glm::mat4 GraphicsObject3DComponent::GetTransform() const
{
	return graphics->GetTransform();
}

void GraphicsObject3DComponent::SetTransform(const glm::mat4& newTransform)
{
	graphics->SetTransform(newTransform);
}

void GraphicsObject3DComponent::SetPosition(const glm::vec3& position)
{
	graphics->SetTranslation(position);
}

void GraphicsObject3DComponent::SetRotation(const glm::mat4& rotation)
{
	graphics->SetRotation(rotation);
}

void GraphicsObject3DComponent::SetScale(const glm::vec3& scale)
{
	graphics->SetScale(scale);
}

void GraphicsObject3DComponent::Scale(const glm::vec3& scale)
{
	graphics->Scale(scale);
}

void GraphicsObject3DComponent::Rotate(float angle, const glm::vec3& axis)
{
	graphics->Rotate(angle, axis);
}

void GraphicsObject3DComponent::Translate(const glm::vec3& translation)
{
	graphics->Translate(translation);
}

void GraphicsObject3DComponent::Update()
{
}

const Model* const GraphicsObject3DComponent::GetModel() const
{
	return graphics->GetModel();
}

GO3D* const GraphicsObject3DComponent::GetGraphics() const
{
	return graphics;
}

void GraphicsObject3DComponent::Disable()
{
	GraphicsObjectManager::Disable(graphics);
}

void GraphicsObject3DComponent::Enable()
{
	GraphicsObjectManager::Enable(graphics);
}
