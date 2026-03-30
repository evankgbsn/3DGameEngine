#ifndef GRAPHICSOBJECTLINE_H
#define GRAPHICSOBJECTLINE_H

#include "Component.h"

class GOLineColored;

class GraphicsObjectLine : public Component
{

public:

	GraphicsObjectLine() = default;

	GraphicsObjectLine(const glm::vec3& point0, const glm::vec3& point1, const glm::vec4& color);

	~GraphicsObjectLine();

	void SetColor(const glm::vec4& color);

	glm::vec4 GetColor() const;

	void SetLineWidth(float width);

	float GetLineWidth() const;

	void SetStart(const glm::vec3& point0);

	void SetEnd(const glm::vec3& point1);

	glm::vec3 GetStart() const;

	glm::vec3 GetEnd() const;

private:

	GraphicsObjectLine(const GraphicsObjectLine&) = delete;

	GraphicsObjectLine& operator=(const GraphicsObjectLine&) = delete;

	GraphicsObjectLine(GraphicsObjectLine&&) = delete;

	GraphicsObjectLine& operator=(GraphicsObjectLine&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	GOLineColored* line;

};

#endif // GRAPHICSOBJECTLINE_H