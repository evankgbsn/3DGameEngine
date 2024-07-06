#ifndef AXISALIGNEDBOUNDINGBOXWITHVISUALIZATION_H
#define AXISALIGNEDBOUNDINGBOXWITHVISUALIZATION_H

#include "../Math/Shapes/AxisAlignedBoundingBox.h"

class GOColoredInstanced;

class AxisAlignedBoundingBoxWithVisualization : public AxisAlignedBoundingBox
{
public:

	AxisAlignedBoundingBoxWithVisualization(const glm::vec3& initialMin = { 0.0f, 0.0f, 0.0f }, const glm::vec3& initialMax = {1.0f, 1.0f, 1.0f});

	void FromOriginAndSize(const glm::vec3& o, const glm::vec3& s);

	~AxisAlignedBoundingBoxWithVisualization();

	void Update();

	void ToggleVisibility();

	void SetColor(const glm::vec4& newColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	const glm::vec4 GetColor() const;

	static void UpdateInstanceTransforms();

	void UpdateGraphicsInstance();

private:

	AxisAlignedBoundingBoxWithVisualization(const AxisAlignedBoundingBoxWithVisualization&) = delete;

	AxisAlignedBoundingBoxWithVisualization& operator=(const AxisAlignedBoundingBoxWithVisualization&) = delete;

	AxisAlignedBoundingBoxWithVisualization(AxisAlignedBoundingBoxWithVisualization&&) = delete;

	AxisAlignedBoundingBoxWithVisualization& operator=(AxisAlignedBoundingBoxWithVisualization&&) = delete;

	void CreateGraphics();

	static GOColoredInstanced* graphics;

	static unsigned int instanceIDGenerator;

	unsigned int instanceID;


};

#endif // AXISALIGNEDBOUNDINGBOXWITHVISUALIZATION_H