#ifndef CAMERACOMPONENT_H
#define CAMERACOMPOENNT_H

#include "Component.h"

#include <glm/glm.hpp>

#include <string>

class CameraComponent : public Component
{

public:

	CameraComponent(const std::string& name);

	~CameraComponent();

	void SetActive() const;

	void Rotate(const glm::vec3& axis, const float& angle);

	void Translate(const glm::vec3& translation);

	glm::vec3 GetForwardVector() const;

	glm::vec3 GetRightVector() const;

	glm::vec3 GetUpVector() const;

	const float& GetFOV() const;

	const float& GetNear() const;

	const float& GetFar() const;

	const glm::vec3& GetPosition() const;

	const glm::mat4& GetView() const;

	const glm::mat4& GetProjection() const;

	void SetPosition(const glm::vec3& position);

	void SetTarget(const glm::vec3& newTarget);

	void SetView(const glm::mat4& newView);

	void SetProjection(const glm::mat4& newProjection);

	void SetFOV(const float& newFOV);

	void SetAspectRatio(const float& newAspectRatio);

	void SetNear(const float& newNear);

	void SetFar(const float& newFar);

private:


	CameraComponent(const CameraComponent&) = delete;

	CameraComponent& operator=(const CameraComponent&) = delete;

	CameraComponent(CameraComponent&&) = delete;

	CameraComponent& operator=(CameraComponent&&) = delete;

	std::string name;

};

#endif // CAMERACOMPONENT_H