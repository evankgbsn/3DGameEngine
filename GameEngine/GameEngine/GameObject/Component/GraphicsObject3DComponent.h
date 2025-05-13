#ifndef GRAPHICSOBJECT3DCOMPONENT_H
#define GRAPHICSOBJECT3DCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class GO3D;
class Model;

class GraphicsObject3DComponent : public Component
{

public:

	glm::vec3 GetPosition() const;

	glm::mat4 GetRotation() const;

	glm::vec3 GetScale() const;

	glm::mat4 GetTransform() const;

	void SetPosition(const glm::vec3& position);

	void SetRotation(const glm::mat4& rotation);

	void SetScale(const glm::vec3& scale);

	void Scale(const glm::vec3& scale);

	void Rotate(float angle, const glm::vec3& axis);

	void Translate(const glm::vec3& translation);

	virtual void Update() override;

	GO3D* const GetGraphics() const;

	const Model* const GetModel() const;

	void Disable();

	void Enable();

protected:

	GraphicsObject3DComponent();

	virtual ~GraphicsObject3DComponent();

	virtual const std::vector<char> Serialize() const override;

	virtual void Deserialize(const std::vector<char>& data) override;

	GO3D* graphics;

private:

	GraphicsObject3DComponent(const GraphicsObject3DComponent&) = delete;

	GraphicsObject3DComponent& operator=(const GraphicsObject3DComponent&) = delete;

	GraphicsObject3DComponent(GraphicsObject3DComponent&&) = delete;

	GraphicsObject3DComponent& operator=(GraphicsObject3DComponent&&) = delete;
};


#endif // GRAPHICSOBJECT3DCOMPONENT_H