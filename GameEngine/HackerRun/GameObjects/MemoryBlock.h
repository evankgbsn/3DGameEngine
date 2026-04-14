#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectColored;
class RigidBodyComponent;
class Text3D;

class MemoryBlock : public GameObject
{
public:

	MemoryBlock();

	~MemoryBlock();

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& pos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& rot) override;

	glm::mat4 GetTransform() const override;

private:

	MemoryBlock(const MemoryBlock&) = delete;

	MemoryBlock& operator=(const MemoryBlock&) = delete;

	MemoryBlock(MemoryBlock&&) = delete;

	MemoryBlock& operator=(MemoryBlock&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	GraphicsObjectColored* graphics;

	RigidBodyComponent* body;

	std::vector<std::vector<Text3D*>> binaryEffect;

	Text3D* binary;

	float lastUpdateTime;
};

#endif