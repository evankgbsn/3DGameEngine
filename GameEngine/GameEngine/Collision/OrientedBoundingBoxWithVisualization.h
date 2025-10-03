#ifndef ORIENTEDBOUNDINGBOXWITHVISUALIZATION_H
#define ORIENTEDBOUNDINGBOXWITHVISUALIZATION_H

#include "../Math/Shapes/OrientedBoundingBox.h"

class GOColored;

class OrientedBoundingBoxWithVisualization : public OrientedBoundingBox
{
public:

	OrientedBoundingBoxWithVisualization(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation = glm::mat4(1.0f));

	OrientedBoundingBoxWithVisualization(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation = glm::mat4(1.0f));

	~OrientedBoundingBoxWithVisualization();

	void SetOffset(const glm::vec3& offset);

	void Update(const glm::mat4& transform);

	void ToggleVisibility();

	bool IsVisible() const;

	void SetColor(const glm::vec4& newColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	const glm::vec4 GetColor() const;

private:

	void CreateGraphics();

	GOColored* graphics;

};

#endif // ORIENTEDBOUNDINGBOXWITHVISUALIZATION_H