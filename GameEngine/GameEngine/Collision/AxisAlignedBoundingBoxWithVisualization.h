#ifndef AXISALIGNEDBOUNDINGBOXWITHVISUALIZATION_H
#define AXISALIGNEDBOUNDINGBOXWITHVISUALIZATION_H

#include "../Math/Shapes/AxisAlignedBoundingBox.h"

class GOColored;

class AxisAlignedBoundingBoxWithVisualization : public AxisAlignedBoundingBox
{
public:

	AxisAlignedBoundingBoxWithVisualization(const glm::vec3& initialMin, const glm::vec3& initialMax);

	~AxisAlignedBoundingBoxWithVisualization();

	void Update(const glm::vec3& newOrigin);

	void ToggleVisibility();

	void SetColor(const glm::vec4& newColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	const glm::vec4 GetColor() const;

private:

	AxisAlignedBoundingBoxWithVisualization(const AxisAlignedBoundingBoxWithVisualization&) = delete;

	AxisAlignedBoundingBoxWithVisualization& operator=(const AxisAlignedBoundingBoxWithVisualization&) = delete;

	AxisAlignedBoundingBoxWithVisualization(AxisAlignedBoundingBoxWithVisualization&&) = delete;

	AxisAlignedBoundingBoxWithVisualization& operator=(AxisAlignedBoundingBoxWithVisualization&&) = delete;

	void CreateGraphics();

	GOColored* graphics;


};

#endif // AXISALIGNEDBOUNDINGBOXWITHVISUALIZATION_H