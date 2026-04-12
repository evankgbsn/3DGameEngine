#ifndef CORRUPTION_H
#define CORRUPTION_H

#include "GameEngine/GameObject/GameObject.h"

#include <list>

class CameraComponent;
class GraphicsObjectColored;

class Corruption : public GameObject
{

public:

	Corruption();

	~Corruption();

private:

	Corruption(const Corruption&) = delete;

	Corruption& operator=(const Corruption&) = delete;

	Corruption(Corruption&&) = delete;

	Corruption& operator=(Corruption&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	GraphicsObjectColored* graphics;
};

#endif