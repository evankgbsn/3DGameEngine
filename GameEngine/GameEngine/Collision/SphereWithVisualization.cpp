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
	GraphicsObjectManager::Delete(graphics);
}

void SphereWithVisualization::Update(const glm::mat4& transformation)
{


	glm::mat4 rotation(1.0f);
	rotation[0] = glm::normalize(transformation[0]);
	rotation[1] = glm::normalize(transformation[1]);
	rotation[2] = glm::normalize(transformation[2]);

	glm::vec3 origin = rotation * glm::vec4(GetOffset(), 1.0f);

	origin = glm::vec3(transformation[3]) + origin;
	graphics->SetTranslation(origin);

	Transform(transformation);
}

void SphereWithVisualization::SetColor(const glm::vec3& color)
{
	graphics->SetColor(glm::vec4(color, 1.0f));
}

void SphereWithVisualization::InitializeGraphics()
{
	graphics = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel("Sphere"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	graphics->SetScale({ GetRadius(), GetRadius(), GetRadius() });
	graphics->SetDrawMode(GO3D::Mode::LINE);
}
