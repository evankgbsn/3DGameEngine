#include "GraphicsObjectLine.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOLineColored.h"

GraphicsObjectLine::GraphicsObjectLine(const glm::vec3& point0, const glm::vec3& point1, const glm::vec4& c)
{
	line = GraphicsObjectManager::CreateGOLineColored(point0, point1, c);
}

GraphicsObjectLine::~GraphicsObjectLine()
{
	GraphicsObjectManager::Delete(line);
}

void GraphicsObjectLine::SetColor(const glm::vec4& color)
{
	line->SetColor(color);
}

glm::vec4 GraphicsObjectLine::GetColor() const
{
	return line->GetColor();
}

void GraphicsObjectLine::SetLineWidth(float width)
{
	line->SetLineWidth(width);
}

float GraphicsObjectLine::GetLineWidth() const
{
	return line->GetLineWidth();
}

void GraphicsObjectLine::SetStart(const glm::vec3& point0)
{
	line->SetStart(point0);
}

void GraphicsObjectLine::SetEnd(const glm::vec3& point1)
{
	line->SetEnd(point1);
}

glm::vec3 GraphicsObjectLine::GetStart() const
{
	return line->GetStart();
}

glm::vec3 GraphicsObjectLine::GetEnd() const
{
	return line->GetEnd();
}

glm::vec3 GraphicsObjectLine::GetDirection() const
{
	return glm::normalize(line->GetEnd() - line->GetStart());
}

void GraphicsObjectLine::Serialize()
{
	savedVec3s["Point0"] = GetStart();
	savedVec3s["Point1"] = GetEnd();
	savedFloats["Width"] = GetLineWidth();
	savedVec4s["Color"] = GetColor();
}

void GraphicsObjectLine::Deserialize()
{
	SetStart(savedVec3s["Point0"]);
	SetEnd(savedVec3s["Point1"]);
	SetLineWidth(savedFloats["Width"]);
	SetColor(savedVec4s["Color"]);
}

void GraphicsObjectLine::Update()
{
}
