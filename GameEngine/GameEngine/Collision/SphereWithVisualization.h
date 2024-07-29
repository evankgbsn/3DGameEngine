#ifndef SPHEREWITHVISUALIZATION_H
#define SPHEREWITHVISUALIZATION_H

#include "../Math/Shapes/Sphere.h"

class GraphicsObject;
class GOColored;

class SphereWithVisualization : public Sphere
{
public:

	SphereWithVisualization(GraphicsObject* go);

	SphereWithVisualization(float radius);

	~SphereWithVisualization();

	void Update(const glm::mat4& transform);

	void SetColor(const glm::vec3& color);

	void ToggleVisibility();

private:

	void InitializeGraphics();

	SphereWithVisualization() = delete;

	SphereWithVisualization(const SphereWithVisualization&) = delete;

	SphereWithVisualization& operator=(const SphereWithVisualization&) = delete;

	SphereWithVisualization(SphereWithVisualization&&) = delete;

	SphereWithVisualization& operator=(SphereWithVisualization&&) = delete;

	GraphicsObject* wrapedGraphics;

	GOColored* graphics;

};

#endif // SPHEREWITHVISUALIZATION_H