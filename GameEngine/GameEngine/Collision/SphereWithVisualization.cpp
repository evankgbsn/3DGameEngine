#include "SphereWithVisualization.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/GraphicsObjects/GraphicsObject.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"

SphereWithVisualization::SphereWithVisualization(GraphicsObject* go) :
	Sphere(go->GetModel()->GetVertices()),
	wrapedGraphics(go)
{
	InitializeGraphics();
}

SphereWithVisualization::SphereWithVisualization(float radius) :
	Sphere(ModelManager::GetModel("Sphere")->GetVertices())
{
	SetRadius(radius);
	InitializeGraphics();
}

SphereWithVisualization::~SphereWithVisualization()
{
}

void SphereWithVisualization::Update(const glm::mat4& transformation)
{
	glm::mat4 scale(1.0f);
	scale = glm::scale(scale, { GetRadius(), GetRadius(), GetRadius() });

	glm::mat4 translation(1.0f);
	translation = glm::translate(translation, GetOffset());

	graphics->SetTransform(transformation * translation * scale);
	Transform(graphics->GetTransform());
}

void SphereWithVisualization::SetColor(const glm::vec3& color)
{
	graphics->SetColor(glm::vec4(color, 1.0f));
}

void SphereWithVisualization::InitializeGraphics()
{
	graphics = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel("Sphere"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	graphics->SetScale({ GetRadius(), GetRadius(), GetRadius() });
	graphics->SetTranslation(GetOrigin());
	graphics->SetDrawMode(GO3D::Mode::LINE);
}
