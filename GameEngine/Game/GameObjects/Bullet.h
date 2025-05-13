#ifndef BULLET_H
#define BULLET_H

#include "GameEngine/GameObject/GameObject.h"

#include <glm/glm.hpp>

class GraphicsObjectColored;
class RigidBodyComponent;

class Bullet : public GameObject
{

public:

	Bullet();

	~Bullet();

	void SetDirection(const glm::vec3& direction);

	void SetOrigin(const glm::vec3& origin);

	void SetPosition(const glm::vec3& newPos) override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

private:

	Bullet(const Bullet&) = delete;

	Bullet& operator=(const Bullet&) = delete;

	Bullet(Bullet&&) = delete;

	Bullet& operator=(Bullet&&) = delete;

	const std::vector<char> Serialize() const override;
	
	void Deserialize(const std::vector<char>& data) override;
	
	void Initialize() override;
	
	void Terminate() override;
	
	void GameUpdate() override;
	
	void EditorUpdate() override;
	
	void Load() override;
	
	void Unload() override;

	void Start() override;

	GraphicsObjectColored* graphics;

	glm::vec3 direction;

	glm::vec3 origin;

	RigidBodyComponent* body;
};

#endif // BULLET_H