#ifndef SKYSPHERE_H
#define SKYSPHERE_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTextured;

class SkySphere : public GameObject
{
public:

	SkySphere();

	~SkySphere();

private:

	SkySphere(const SkySphere&) = delete;

	SkySphere& operator=(const SkySphere&) = delete;

	SkySphere(SkySphere&&) = delete;

	SkySphere& operator=(SkySphere&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	bool Hovered() const override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetTransform() const override;

	GraphicsObjectTextured* sky;

};

#endif // SKYSPHERE_H