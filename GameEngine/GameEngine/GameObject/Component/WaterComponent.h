#ifndef WaterComponent_H
#define WaterComponent_H

#include "GraphicsObject3DComponent.h"

#include <functional>
#include <string>

class Model;
class Terrain;
class AxisAlignedBoundingBox;
class LineSegment3D;

class WaterComponent : public Component
{
public:

	WaterComponent();

	WaterComponent(const std::string& name, float width, float height, unsigned int tileX, unsigned int tileY, float yOffset);

	~WaterComponent();

	glm::vec3 GetTerrainPoint(const glm::vec3& position) const;

	const std::vector<std::vector<AxisAlignedBoundingBox*>>& GetCellArray() const;

	void ToggleCells();

	bool ColliderVisible() const;

	void ToggleColliderVisibility();

	glm::vec3 GetLineIntersection(const LineSegment3D& line);

	bool Loaded() const;

private:

	WaterComponent(const WaterComponent&) = delete;

	WaterComponent& operator=(const WaterComponent&) = delete;

	WaterComponent(WaterComponent&&) = delete;

	WaterComponent& operator=(WaterComponent&&) = delete;

	void Update() override;

	void Serialize() override;

	void Deserialize() override;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	Terrain* terrain;
};

#endif // GRAPHICSOBJECT_WATER_H