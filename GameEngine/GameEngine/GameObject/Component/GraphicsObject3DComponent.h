#ifndef GRAPHICSOBJECT3DCOMPONENT_H
#define GRAPHICSOBJECT3DCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class GO3D;

class GraphicsObject3DComponent : public Component
{

public:

	glm::vec3 GetPosition() const;

	glm::mat4 GetRotation() const;

	glm::vec3 GetScale() const;

	void SetPosition(const glm::vec3& position);

	void SetRotation(const glm::mat4& rotation);

	void SetScale(const glm::vec3& scale);

	void Scale(const glm::vec3& scale);

	void Rotate(float angle, const glm::vec3& axis);

	void Translate(const glm::vec3& translation);

	virtual void Update() override;

protected:

	GraphicsObject3DComponent();

	virtual ~GraphicsObject3DComponent();

	GO3D* graphics;

private:

	GraphicsObject3DComponent(const GraphicsObject3DComponent&) = delete;

	GraphicsObject3DComponent& operator=(const GraphicsObject3DComponent&) = delete;

	GraphicsObject3DComponent(GraphicsObject3DComponent&&) = delete;

	GraphicsObject3DComponent& operator=(GraphicsObject3DComponent&&) = delete;

};


#endif // GRAPHICSOBJECT3DCOMPONENT_H